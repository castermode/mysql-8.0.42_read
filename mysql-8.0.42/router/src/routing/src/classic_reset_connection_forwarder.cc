/*
  Copyright (c) 2023, 2025, Oracle and/or its affiliates.

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License, version 2.0,
  as published by the Free Software Foundation.

  This program is designed to work with certain software (including
  but not limited to OpenSSL) that is licensed under separate terms,
  as designated in a particular file or component or in included license
  documentation.  The authors of MySQL hereby grant you an additional
  permission to link the program and your derivative works with the
  separately licensed software that they have either included with
  the program or referenced in the documentation.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include "classic_reset_connection_forwarder.h"

#include "classic_connection_base.h"
#include "classic_frame.h"
#include "classic_lazy_connect.h"
#include "classic_query_sender.h"
#include "mysql/harness/logging/logging.h"
#include "mysql/harness/stdx/expected.h"
#include "mysql/harness/tls_error.h"

IMPORT_LOG_FUNCTIONS()

/**
 * forward the reset-connection message flow.
 *
 * Expected overall flow:
 *
 * @code
 * c->s: COM_RESET_CONNECTION
 * c<-s: Ok
 * @endcode
 *
 * If there is no server connection, it is created on demand.
 */
stdx::expected<Processor::Result, std::error_code>
ResetConnectionForwarder::process() {
  switch (stage()) {
    case Stage::Command:
      return command();
    case Stage::Connect:
      return connect();
    case Stage::Connected:
      return connected();
    case Stage::Response:
      return response();
    case Stage::Ok:
      return ok();
    case Stage::Done:
      return Result::Done;
  }

  harness_assert_this_should_not_execute();
}

stdx::expected<Processor::Result, std::error_code>
ResetConnectionForwarder::command() {
  if (auto &tr = tracer()) {
    tr.trace(Tracer::Event().stage("reset_connection::command"));
  }

  auto &server_conn = connection()->socket_splicer()->server_conn();
  if (!server_conn.is_open()) {
    stage(Stage::Connect);
    return Result::Again;
  } else {
    stage(Stage::Response);
    return forward_client_to_server();
  }
}

stdx::expected<Processor::Result, std::error_code>
ResetConnectionForwarder::connect() {
  if (auto &tr = tracer()) {
    tr.trace(Tracer::Event().stage("reset_connection::connect"));
  }

  stage(Stage::Connected);
  return mysql_reconnect_start();
}

stdx::expected<Processor::Result, std::error_code>
ResetConnectionForwarder::connected() {
  auto &server_conn = connection()->socket_splicer()->server_conn();
  if (!server_conn.is_open()) {
    auto *socket_splicer = connection()->socket_splicer();
    auto *src_channel = socket_splicer->client_channel();
    auto *src_protocol = connection()->client_protocol();

    // take the client::command from the connection.
    auto recv_res =
        ClassicFrame::ensure_has_full_frame(src_channel, src_protocol);
    if (!recv_res) return recv_client_failed(recv_res.error());

    discard_current_msg(src_channel, src_protocol);

    if (auto &tr = tracer()) {
      tr.trace(Tracer::Event().stage("reset_connection::connect::error"));
    }

    stage(Stage::Done);
    return reconnect_send_error_msg(src_channel, src_protocol);
  }

  if (auto &tr = tracer()) {
    tr.trace(Tracer::Event().stage("reset_connection::connected"));
  }

  stage(Stage::Response);
  return forward_client_to_server();
}

stdx::expected<Processor::Result, std::error_code>
ResetConnectionForwarder::response() {
  auto *socket_splicer = connection()->socket_splicer();
  auto src_channel = socket_splicer->server_channel();
  auto src_protocol = connection()->server_protocol();

  auto read_res =
      ClassicFrame::ensure_has_msg_prefix(src_channel, src_protocol);
  if (!read_res) return recv_server_failed(read_res.error());

  const uint8_t msg_type = src_protocol->current_msg_type().value();

  enum class Msg {
    Ok = ClassicFrame::cmd_byte<classic_protocol::message::server::Ok>(),
  };

  // reset-connection is not expected to fail.
  switch (Msg{msg_type}) {
    case Msg::Ok:
      stage(Stage::Ok);
      return Result::Again;
  }

  log_debug("reset_connection::response: unexpected msg-type '%02x'", msg_type);

  return stdx::make_unexpected(make_error_code(std::errc::bad_message));
}

stdx::expected<Processor::Result, std::error_code>
ResetConnectionForwarder::ok() {
  auto *socket_splicer = connection()->socket_splicer();
  auto *src_channel = socket_splicer->server_channel();
  auto *src_protocol = connection()->server_protocol();
  auto *dst_protocol = connection()->client_protocol();

  auto msg_res =
      ClassicFrame::recv_msg<classic_protocol::borrowed::message::server::Ok>(
          src_channel, src_protocol);
  if (!msg_res) return recv_server_failed(msg_res.error());

  if (auto &tr = tracer()) {
    tr.trace(Tracer::Event().stage("reset_connection::ok"));
  }

  auto msg = *msg_res;

  if (!msg.session_changes().empty()) {
    auto track_res = connection()->track_session_changes(
        net::buffer(msg.session_changes()),
        src_protocol->shared_capabilities());
  }

  dst_protocol->status_flags(msg.status_flags());

  // allow connection sharing again.
  connection()->connection_sharing_allowed_reset();

  // clear the warnings
  connection()->execution_context().diagnostics_area().warnings().clear();

  // clear the prepared statements.
  connection()->client_protocol()->prepared_statements().clear();

  if (connection()->context().connection_sharing() &&
      connection()->greeting_from_router()) {
    // if connection sharing is enabled in the config, enable the
    // session-tracker.
    connection()->push_processor(std::make_unique<QuerySender>(connection(), R"(
SET @@SESSION.session_track_schema           = 'ON',
    @@SESSION.session_track_system_variables = '*',
    @@SESSION.session_track_transaction_info = 'CHARACTERISTICS',
    @@SESSION.session_track_gtids            = 'OWN_GTID',
    @@SESSION.session_track_state_change     = 'ON')"));

    stage(Stage::Done);
  } else {
    stage(Stage::Done);
  }

  return forward_server_to_client();
}

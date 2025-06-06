/*
 * Copyright (c) 2017, 2025, Oracle and/or its affiliates.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License, version 2.0,
 * as published by the Free Software Foundation.
 *
 * This program is designed to work with certain software (including
 * but not limited to OpenSSL) that is licensed under separate terms,
 * as designated in a particular file or component or in included license
 * documentation.  The authors of MySQL hereby grant you an additional
 * permission to link the program and your derivative works with the
 * separately licensed software that they have either included with
 * the program or referenced in the documentation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License, version 2.0, for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301  USA
 */

#ifndef UNITTEST_GUNIT_XPLUGIN_XCL_MOCK_FACTORY_H_
#define UNITTEST_GUNIT_XPLUGIN_XCL_MOCK_FACTORY_H_

#include <gmock/gmock.h>
#include <memory>

#include "plugin/x/client/xprotocol_factory.h"

namespace xcl {
namespace test {
namespace mock {

class Protocol_factory : public xcl::Protocol_factory {
 public:
  Protocol_factory();
  virtual ~Protocol_factory() override;

  MOCK_METHOD(xcl::XProtocol *, create_protocol_raw,
              (std::shared_ptr<Context> context));
  MOCK_METHOD(xcl::XConnection *, create_connection_raw,
              (std::shared_ptr<Context> context));
  MOCK_METHOD(xcl::XQuery_result *, create_result_raw,
              (std::shared_ptr<XProtocol>, Query_instances *,
               std::shared_ptr<Context>));

 private:
  std::shared_ptr<xcl::XProtocol> create_protocol(
      std::shared_ptr<Context> context) override {
    std::shared_ptr<xcl::XProtocol> result{create_protocol_raw(context)};

    return result;
  }

  std::unique_ptr<xcl::XConnection> create_connection(
      std::shared_ptr<Context> context) override {
    std::unique_ptr<xcl::XConnection> result{create_connection_raw(context)};

    return result;
  }

  std::unique_ptr<xcl::XQuery_result> create_result(
      std::shared_ptr<xcl::XProtocol> protocol,
      Query_instances *query_instances,
      std::shared_ptr<Context> context) override {
    std::unique_ptr<xcl::XQuery_result> result{
        create_result_raw(protocol, query_instances, context)};

    return result;
  }
};

}  // namespace mock
}  // namespace test
}  // namespace xcl

#endif  // UNITTEST_GUNIT_XPLUGIN_XCL_MOCK_FACTORY_H_

/* Copyright (c) 2011, 2025, Oracle and/or its affiliates.

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
   GNU General Public License, version 2.0, for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301  USA */

/**
  @file include/mysql_com_server.h
  Definitions private to the server,
  used in the networking layer to notify specific events.
*/

#ifndef _mysql_com_server_h
#define _mysql_com_server_h

#include <stddef.h>

#include "compression.h"
#include "my_inttypes.h"

typedef void (*before_header_callback_fn)(NET *net, void *user_data,
                                          size_t count);

typedef void (*after_header_callback_fn)(NET *net, void *user_data,
                                         size_t count, bool rc);

/**
  This structure holds the negotiated compression algorithm and level
  between client and server.
*/
struct compression_attributes {
  char compress_algorithm[COMPRESSION_ALGORITHM_NAME_LENGTH_MAX];
  unsigned int compress_level;
  bool compression_optional;
  compression_attributes() {
    compress_algorithm[0] = '\0';
    compress_level = 0;
    compression_optional = false;
  }
};

typedef struct NET_SERVER {
  before_header_callback_fn m_before_header;
  after_header_callback_fn m_after_header;
  void *m_user_data;
  struct compression_attributes compression;
  mysql_compress_context compress_ctx;
  bool timeout_on_full_packet;
} NET_SERVER;

inline void net_server_ext_init(NET_SERVER *ns) {
  ns->m_user_data = nullptr;
  ns->m_before_header = nullptr;
  ns->m_after_header = nullptr;
  ns->compress_ctx.algorithm = MYSQL_UNCOMPRESSED;
  ns->timeout_on_full_packet = false;
}

#endif

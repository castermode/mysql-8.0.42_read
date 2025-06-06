# Copyright (c) 2010, 2025, Oracle and/or its affiliates.
# 
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License, version 2.0,
# as published by the Free Software Foundation.
#
# This program is designed to work with certain software (including
# but not limited to OpenSSL) that is licensed under separate terms,
# as designated in a particular file or component or in included license
# documentation.  The authors of MySQL hereby grant you an additional
# permission to link the program and your derivative works with the
# separately licensed software that they have either included with
# the program or referenced in the documentation.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License, version 2.0, for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301  USA

DISABLE_MISSING_PROFILE_WARNING()

MYSQL_ADD_PLUGIN(auth dialog.cc 
  MODULE_ONLY
  TEST_ONLY
  SERVER_AND_CLIENT
  )
MYSQL_ADD_PLUGIN(auth_test_plugin
  test_plugin.cc 
  MODULE_ONLY
  TEST_ONLY
  SERVER_AND_CLIENT
  )
MYSQL_ADD_PLUGIN(qa_auth_interface
  qa_auth_interface.cc 
  MODULE_ONLY
  TEST_ONLY
  SERVER_AND_CLIENT
  )
MYSQL_ADD_PLUGIN(qa_auth_server
  qa_auth_server.cc 
  MODULE_ONLY
  TEST_ONLY
  )
MYSQL_ADD_PLUGIN(qa_auth_client
  qa_auth_client.cc 
  MODULE_ONLY
  TEST_ONLY
  CLIENT_ONLY
  )
MYSQL_ADD_PLUGIN(mysql_no_login
  mysql_no_login.cc 
  MODULE_ONLY
  )

CHECK_CXX_SOURCE_COMPILES(
"#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include <sys/socket.h>
int main() {
  struct ucred cred;
  getsockopt(0, SOL_SOCKET, SO_PEERCRED, &cred, 0);
}" HAVE_PEERCRED)

IF(HAVE_PEERCRED)
  MYSQL_ADD_PLUGIN(auth_socket
    auth_socket.cc
    MODULE_ONLY
    )
ENDIF()

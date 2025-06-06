# Copyright (c) 2021, 2025, Oracle and/or its affiliates.
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

INCLUDE(${CMAKE_SOURCE_DIR}/cmake/protobuf.cmake)

SET(PROTOBUF_LITE_GENERATE_DIR "${CMAKE_CURRENT_BINARY_DIR}/generated/protobuf_lite")
FILE(MAKE_DIRECTORY ${PROTOBUF_LITE_GENERATE_DIR})

SET(MY_PROTOBUF_PROTO_FILES
  "${CMAKE_CURRENT_SOURCE_DIR}/replication_asynchronous_connection_failover.proto"
)

MYSQL_PROTOBUF_GENERATE_CPP_LIBRARY(
  rpl_protobuf_lite
  ${MY_PROTOBUF_PROTO_FILES}
  OUTPUT_DIRECTORY ${PROTOBUF_LITE_GENERATE_DIR}
  ADDITIONAL_COMMENT "- lite version")

TARGET_LINK_LIBRARIES(rpl_protobuf_lite PUBLIC ext::libprotobuf-lite)

SET(RPL_PROTOBUF_LITE_LIB rpl_protobuf_lite PARENT_SCOPE)

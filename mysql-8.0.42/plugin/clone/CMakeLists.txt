# Copyright (c) 2018, 2025, Oracle and/or its affiliates.
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
MSVC_CPPCHECK_DISABLE()

ADD_DEFINITIONS(-DMYSQL_SERVER)
ADD_DEFINITIONS(-DLOG_SUBSYSTEM_TAG="Clone")
ADD_DEFINITIONS(-DLOG_COMPONENT_TAG="Clone")

MYSQL_ADD_PLUGIN(clone
  src/clone_plugin.cc
  src/clone_client.cc
  src/clone_server.cc
  src/clone_status.cc
  src/clone_local.cc
  src/clone_os.cc
  src/clone_hton.cc
  MODULE_ONLY
  MODULE_OUTPUT_NAME "mysql_clone"
  )

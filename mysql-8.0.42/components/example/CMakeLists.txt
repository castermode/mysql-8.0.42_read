# Copyright (c) 2016, 2025, Oracle and/or its affiliates.
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

MYSQL_ADD_COMPONENT(example_component1
  english_greeting_service_imp.cc
  example_component1.cc
  simple_example_math_imp.cc
  MODULE_ONLY
  TEST_ONLY
  )
MYSQL_ADD_COMPONENT(example_component2
  example_component2.cc
  polish_greeting_service_imp.cc
  MODULE_ONLY
  TEST_ONLY
  )
MYSQL_ADD_COMPONENT(example_component3
  example_component3.cc
  example_math_wrapping_imp.cc
  MODULE_ONLY
  TEST_ONLY
  )
MYSQL_ADD_COMPONENT(test_string_service
  test_string_service.cc
  MODULE_ONLY
  TEST_ONLY
  )
MYSQL_ADD_COMPONENT(test_string_service_long
  test_string_service_long.cc
  MODULE_ONLY
  TEST_ONLY
  )
MYSQL_ADD_COMPONENT(test_string_service_charset
  test_string_service_charset.cc
  MODULE_ONLY
  TEST_ONLY
  )
MYSQL_ADD_COMPONENT(test_backup_lock_service
  test_backup_lock_service.cc
  MODULE_ONLY
  TEST_ONLY
  )

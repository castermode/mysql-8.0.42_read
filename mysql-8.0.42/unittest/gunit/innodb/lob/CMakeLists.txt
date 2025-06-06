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

SET(ZLOB_SRC_FILES
  zlob0int-t.cc
  zlob0int.cc
  zlob0int.h
  mtr0log.cc
  ut0dbg.cc
  lot0buf.cc
  fil0fil.cc
  fut0lst.cc
  lot0dat.cc)

# Plain LOB (or Uncompressed LOB) source files.
SET(PLOB_SRC_FILES
  lot0lot.cc
  lot0buf.cc
  lot0dat.cc
  lot0dat.h
  lot0lob.h
  lot0lob.cc
  mach0data.h
  fut0lst.cc
  fil0fil.cc
  mtr0log.cc
  ut0dbg.cc
  lob0int.cc)

MYSQL_ADD_EXECUTABLE(lot0lot-t ${PLOB_SRC_FILES}
  SKIP_INSTALL
  ENABLE_EXPORTS
  LINK_LIBRARIES ext::zlib ${GTEST_LIBRARIES} gunit_small
  )
MYSQL_ADD_EXECUTABLE(ut0plist-t
  lot0buf.cc
  mtr0log.cc
  ut0dbg.cc
  ut0plist.cc
  SKIP_INSTALL
  )
MYSQL_ADD_EXECUTABLE(ut0frags-t
  fil0fil.cc
  fut0lst.cc
  lot0buf.cc
  mtr0log.cc
  ut0dbg.cc
  ut0frags.cc
  zlob0int.cc
  SKIP_INSTALL
  LINK_LIBRARIES ext::zlib
  )
MYSQL_ADD_EXECUTABLE(zlob0int-t ${ZLOB_SRC_FILES}
  SKIP_INSTALL
  ENABLE_EXPORTS
  LINK_LIBRARIES ext::zlib ${GTEST_LIBRARIES} gunit_small
  )

# Don't run these by default, run manually
# ADD_TEST(lot0lot  lot0lot-t)
# ADD_TEST(ut0plist ut0plist-t)
# ADD_TEST(ut0frags ut0frags-t)
# ADD_TEST(zlob0int zlob0int-t)

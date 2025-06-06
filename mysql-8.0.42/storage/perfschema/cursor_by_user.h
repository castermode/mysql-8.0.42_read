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
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301  USA
  */

#ifndef CURSOR_BY_USER_H
#define CURSOR_BY_USER_H

/**
  @file storage/perfschema/cursor_by_user.h
  Cursor CURSOR_BY_USER (declarations).
*/

#include "storage/perfschema/pfs_engine_table.h"
#include "storage/perfschema/pfs_user.h"
#include "storage/perfschema/table_helper.h"

/**
  @addtogroup performance_schema_tables
  @{
*/

class PFS_index_users : public PFS_engine_index {
 public:
  explicit PFS_index_users(PFS_engine_key *key_1) : PFS_engine_index(key_1) {}

  ~PFS_index_users() override = default;

  virtual bool match(PFS_user *pfs) = 0;
};

/** Cursor CURSOR_BY_USER. */
class cursor_by_user : public PFS_engine_table {
 public:
  static ha_rows get_row_count();

  void reset_position() override;

  int rnd_next() override;
  int rnd_pos(const void *pos) override;

  int index_next() override;

 protected:
  explicit cursor_by_user(const PFS_engine_table_share *share);

 public:
  ~cursor_by_user() override = default;

 protected:
  virtual int make_row(PFS_user *user) = 0;

 private:
  /** Current position. */
  PFS_simple_index m_pos;
  /** Next position. */
  PFS_simple_index m_next_pos;

 protected:
  PFS_index_users *m_opened_index;
};

/** @} */
#endif

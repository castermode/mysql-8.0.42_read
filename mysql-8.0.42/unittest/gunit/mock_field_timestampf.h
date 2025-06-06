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

#ifndef MOCK_FIELD_TIMESTAMPF_H
#define MOCK_FIELD_TIMESTAMPF_H

#include "sql/field.h"
#include "unittest/gunit/fake_table.h"

class Mock_field_timestampf : public Field_timestampf {
  void initialize() {
    table = new Fake_TABLE(this);
    EXPECT_FALSE(table == nullptr) << "Out of memory";
    ptr = table->record[0] + 1;
    set_null_ptr(table->record[0], 1);
  }

 public:
  bool store_timestamp_internal_called;
  Mock_field_timestampf(uchar auto_flags_arg, int scale)
      : Field_timestampf(nullptr,                     // ptr_arg
                         nullptr,                     // null_ptr_arg
                         '\0',                        // null_bit_arg
                         auto_flags_arg,              // auto_flags_arg
                         "",                          // field_name_arg
                         static_cast<uint8>(scale)),  // dec_arg a.k.a. scale.
        store_timestamp_internal_called(false) {
    initialize();
  }

  my_timeval to_timeval() {
    my_timeval tm;
    int warnings = 0;
    get_timestamp(&tm, &warnings);
    EXPECT_EQ(0, warnings);
    return tm;
  }

  /* Averts ASSERT_COLUMN_MARKED_FOR_WRITE assertion. */
  void make_writable() { bitmap_set_bit(table->write_set, field_index()); }

  void store_timestamp_internal(const my_timeval *tm) override {
    store_timestamp_internal_called = true;
    return Field_timestampf::store_timestamp_internal(tm);
  }

  ~Mock_field_timestampf() override { delete static_cast<Fake_TABLE *>(table); }
};

#endif  // MOCK_FIELD_TIMESTAMPF_H

/* Copyright (c) 2014, 2025, Oracle and/or its affiliates.

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

#ifndef DD__FOREIGN_KEY_ELEMENT_IMPL_INCLUDED
#define DD__FOREIGN_KEY_ELEMENT_IMPL_INCLUDED

#include <stddef.h>
#include <sys/types.h>
#include <new>

#include "sql/dd/impl/types/weak_object_impl.h"  // dd::Weak_object_impl
#include "sql/dd/sdi_fwd.h"
#include "sql/dd/string_type.h"
#include "sql/dd/types/foreign_key_element.h"  // dd::Foreign_key_element

namespace dd {

///////////////////////////////////////////////////////////////////////////

class Column;
class Foreign_key;
class Foreign_key_impl;
class Object_key;
class Object_table;
class Open_dictionary_tables_ctx;
class Raw_record;
class Sdi_rcontext;
class Sdi_wcontext;
class Weak_object;

///////////////////////////////////////////////////////////////////////////

class Foreign_key_element_impl : public Weak_object_impl,
                                 public Foreign_key_element {
 public:
  Foreign_key_element_impl()
      : m_foreign_key(nullptr), m_column(nullptr), m_ordinal_position(0) {}

  Foreign_key_element_impl(Foreign_key_impl *foreign_key)
      : m_foreign_key(foreign_key), m_column(nullptr), m_ordinal_position(0) {}

  Foreign_key_element_impl(const Foreign_key_element_impl &src,
                           Foreign_key_impl *parent, Column *column);

  ~Foreign_key_element_impl() override = default;

 public:
  const Object_table &object_table() const override;

  static void register_tables(Open_dictionary_tables_ctx *otx);

  bool validate() const override;

  bool restore_attributes(const Raw_record &r) override;

  bool store_attributes(Raw_record *r) override;

  void serialize(Sdi_wcontext *wctx, Sdi_writer *w) const override;

  bool deserialize(Sdi_rcontext *rctx, const RJ_Value &val) override;

  void debug_print(String_type &outb) const override;

  void set_ordinal_position(uint ordinal_position) {
    m_ordinal_position = ordinal_position;
  }

 public:
  /////////////////////////////////////////////////////////////////////////
  // Foreign key.
  /////////////////////////////////////////////////////////////////////////

  const Foreign_key &foreign_key() const override;

  Foreign_key &foreign_key() override;

  /////////////////////////////////////////////////////////////////////////
  // column.
  /////////////////////////////////////////////////////////////////////////

  const Column &column() const override { return *m_column; }

  void set_column(const Column *column) override { m_column = column; }

  /////////////////////////////////////////////////////////////////////////
  // ordinal_position.
  /////////////////////////////////////////////////////////////////////////

  uint ordinal_position() const override { return m_ordinal_position; }

  void set_ordinal_position(int ordinal_position) override {
    m_ordinal_position = ordinal_position;
  }

  /////////////////////////////////////////////////////////////////////////
  // referenced column name.
  /////////////////////////////////////////////////////////////////////////

  const String_type &referenced_column_name() const override {
    return m_referenced_column_name;
  }

  void referenced_column_name(const String_type &name) override {
    m_referenced_column_name = name;
  }

 public:
  static Foreign_key_element_impl *restore_item(Foreign_key_impl *fk) {
    return new (std::nothrow) Foreign_key_element_impl(fk);
  }

  static Foreign_key_element_impl *clone(const Foreign_key_element_impl &other,
                                         Foreign_key_impl *fk);

 public:
  Object_key *create_primary_key() const override;
  bool has_new_primary_key() const override;

 private:
  Foreign_key_impl *m_foreign_key;
  const Column *m_column;
  uint m_ordinal_position;
  String_type m_referenced_column_name;
};

///////////////////////////////////////////////////////////////////////////

}  // namespace dd

#endif  // DD__FOREIGN_KEY_ELEMENT_IMPL_INCLUDED

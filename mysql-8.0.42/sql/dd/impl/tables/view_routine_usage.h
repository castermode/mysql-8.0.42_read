/* Copyright (c) 2016, 2025, Oracle and/or its affiliates.

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

#ifndef DD_TABLES__VIEW_ROUTINE_USAGE_INCLUDED
#define DD_TABLES__VIEW_ROUTINE_USAGE_INCLUDED

#include "sql/dd/impl/types/object_table_impl.h"  // dd::Object_table_impl
#include "sql/dd/object_id.h"                     // dd::Object_id
#include "sql/dd/string_type.h"

namespace dd {
class Object_key;

namespace tables {

///////////////////////////////////////////////////////////////////////////

class View_routine_usage : virtual public Object_table_impl {
 public:
  static const View_routine_usage &instance();

  enum enum_fields {
    FIELD_VIEW_ID,
    FIELD_ROUTINE_CATALOG,
    FIELD_ROUTINE_SCHEMA,
    FIELD_ROUTINE_NAME,
    NUMBER_OF_FIELDS  // Always keep this entry at the end of the enum
  };

  enum enum_indexes {
    INDEX_PK_VIEW_ID_ROUTINE_CATALOG,
    INDEX_K_ROUTINE_CATALOG_ROUTINE_SCHEMA_ROUTINE_NAME
  };

  enum enum_foreign_keys { FK_VIEW_ID };

  View_routine_usage();

  static Object_key *create_key_by_view_id(Object_id view_id);

  static Object_key *create_primary_key(Object_id view_id,
                                        const String_type &routine_catalog,
                                        const String_type &routine_schema,
                                        const String_type &routine_name);

  static Object_key *create_key_by_name(const String_type &routine_catalog,
                                        const String_type &routine_schema,
                                        const String_type &routine_name);
};

///////////////////////////////////////////////////////////////////////////

}  // namespace tables
}  // namespace dd

#endif  // DD_TABLES__VIEW_ROUTINE_USAGE_INCLUDED

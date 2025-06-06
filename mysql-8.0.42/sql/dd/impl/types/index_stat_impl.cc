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

#include "sql/dd/impl/types/index_stat_impl.h"

#include <ostream>
#include <string>

#include "my_sys.h"
#include "mysqld_error.h"
#include "sql/dd/impl/raw/object_keys.h"     // Composite_4char_key
#include "sql/dd/impl/raw/raw_record.h"      // raw_record
#include "sql/dd/impl/tables/index_stats.h"  // Index_stats::
#include "sql/dd/impl/transaction_impl.h"    // Open_dictionary_tables_ctx

namespace dd {
class Object_key;
}  // namespace dd

using dd::tables::Index_stats;

namespace dd {

///////////////////////////////////////////////////////////////////////////
// Index_stat_impl implementation.
///////////////////////////////////////////////////////////////////////////

bool Index_stat_impl::has_new_primary_key() const {
  /*
    There is no OBJECT_ID for Table_stat/Index_stat DD object.
    So deciding if a object exists or not is not possible based
    on just schema and table name. We would need to add a new
    numeric object id for the same. Adding this new column to
    these objects would be unnecessary and serve the only purpose to
    update or insert the objects. Additionally, it would cost little
    more disk space too.

    These DD objects are only updated. That is, if the row exists, we
    just update it; otherwise, we insert a new row. Returning 'false' here
    enables expected behavior. Even if we have added a new
    numeric primary key, that would require to first fetch the DD
    object and then call store(). Instead of returning false here
    will end-up doing the same, we would first try to find the
    object and then insert if not found.
  */

  return false;
}

///////////////////////////////////////////////////////////////////////////
// Index_stat_impl implementation.
///////////////////////////////////////////////////////////////////////////

bool Index_stat_impl::validate() const {
  if (schema_name().empty() || table_name().empty()) {
    my_error(ER_INVALID_DD_OBJECT, MYF(0),
             Index_stat_impl::DD_table::instance().name().c_str(),
             "schema name or table name not supplied.");
    return true;
  }

  return false;
}

///////////////////////////////////////////////////////////////////////////

bool Index_stat_impl::restore_attributes(const Raw_record &r) {
  m_schema_name = r.read_str(Index_stats::FIELD_SCHEMA_NAME);
  m_table_name = r.read_str(Index_stats::FIELD_TABLE_NAME);
  m_index_name = r.read_str(Index_stats::FIELD_INDEX_NAME);
  m_column_name = r.read_str(Index_stats::FIELD_COLUMN_NAME);
  m_cardinality = r.read_int(Index_stats::FIELD_CARDINALITY);
  m_cached_time = r.read_int(Index_stats::FIELD_CACHED_TIME);

  return false;
}

///////////////////////////////////////////////////////////////////////////

bool Index_stat_impl::store_attributes(Raw_record *r) {
  return r->store(Index_stats::FIELD_SCHEMA_NAME, m_schema_name) ||
         r->store(Index_stats::FIELD_TABLE_NAME, m_table_name) ||
         r->store(Index_stats::FIELD_INDEX_NAME, m_index_name) ||
         r->store(Index_stats::FIELD_COLUMN_NAME, m_column_name) ||
         r->store(Index_stats::FIELD_CARDINALITY, m_cardinality,
                  m_cardinality == (ulonglong)-1) ||
         r->store(Index_stats::FIELD_CACHED_TIME, m_cached_time);
}

///////////////////////////////////////////////////////////////////////////

void Index_stat_impl::debug_print(String_type &outb) const {
  dd::Stringstream_type ss;
  ss << "INDEX STAT OBJECT: { "
     << "m_schema_name: " << m_schema_name << "; "
     << "m_table_name: " << m_table_name << "; "
     << "m_index_name: " << m_index_name << "; "
     << "m_column_name: " << m_column_name << "; "
     << "m_cardinality: " << m_cardinality << "; "
     << "m_cached_time: " << m_cached_time;

  ss << " }";
  outb = ss.str();
}

///////////////////////////////////////////////////////////////////////////

Object_key *Index_stat_impl::create_primary_key() const {
  return dynamic_cast<Object_key *>(Index_stats::create_object_key(
      m_schema_name, m_table_name, m_index_name, m_column_name));
}

///////////////////////////////////////////////////////////////////////////

const Object_table &Index_stat_impl::object_table() const {
  return DD_table::instance();
}

///////////////////////////////////////////////////////////////////////////

void Index_stat_impl::register_tables(Open_dictionary_tables_ctx *otx) {
  /**
    The requirement is that we should be able to update
    Table_stats and Index_stats DD tables even when someone holds
    global read lock, when we execute ANALYZE TABLE.
  */
  otx->mark_ignore_global_read_lock();
  otx->add_table<Index_stats>();
}

///////////////////////////////////////////////////////////////////////////

}  // namespace dd

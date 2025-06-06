/*
  Copyright (c) 2015, 2025, Oracle and/or its affiliates.

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

#ifndef TRIGGER_INCLUDED
#define TRIGGER_INCLUDED

#include "client/dump/abstract_plain_sql_object_dump_task.h"
#include "client/dump/table.h"
#include "my_inttypes.h"

namespace Mysql {
namespace Tools {
namespace Dump {

class Trigger : public Abstract_plain_sql_object_dump_task {
 public:
  Trigger(uint64 id, const std::string &name, const std::string &schema,
          const std::string &sql_formatted_definition,
          const Table *defined_table);
  const Table *get_defined_table();

 private:
  /* Holds table object based on which this trigger is defined. */
  const Table *m_defined_table;
};

}  // namespace Dump
}  // namespace Tools
}  // namespace Mysql

#endif

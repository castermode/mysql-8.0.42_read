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

#ifndef ABSTRACT_DATABASE_DUMP_TASK_INCLUDED
#define ABSTRACT_DATABASE_DUMP_TASK_INCLUDED

#include "client/dump/abstract_dump_task.h"
#include "client/dump/database.h"

namespace Mysql {
namespace Tools {
namespace Dump {

/**
  Abstract class for defining single database definition dump task.
 */
class Abstract_database_dump_task : public Abstract_dump_task {
 public:
  explicit Abstract_database_dump_task(Database *related_database);

  /**
    Returns database the current task is created for.
   */
  Database *get_related_database();
};

}  // namespace Dump
}  // namespace Tools
}  // namespace Mysql

#endif

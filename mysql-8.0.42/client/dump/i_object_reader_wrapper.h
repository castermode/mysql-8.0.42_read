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

#ifndef I_OBJECT_READER_WRAPPER_INCLUDED
#define I_OBJECT_READER_WRAPPER_INCLUDED

#include "client/dump/i_object_reader.h"

namespace Mysql {
namespace Tools {
namespace Dump {

/**
  Represents class that directs execution of dump tasks to Object Readers.
 */
class I_object_reader_wrapper {
 public:
  virtual ~I_object_reader_wrapper() = default;
  /**
    Add new Object Reader to supply direct execution of dump tasks to.
   */
  virtual void register_object_reader(I_object_reader *new_object_reader) = 0;
};

}  // namespace Dump
}  // namespace Tools
}  // namespace Mysql

#endif

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

#ifndef MYSQL_OBJECT_READER_OPTIONS_INCLUDED
#define MYSQL_OBJECT_READER_OPTIONS_INCLUDED

#include "client/base/abstract_options_provider.h"
#include "client/dump/mysql_chain_element_options.h"
#include "my_inttypes.h"

namespace Mysql {
namespace Tools {
namespace Dump {

#define MAX_EXTENDED_INSERT 0x100000

class Mysql_object_reader_options
    : public Mysql::Tools::Base::Options::Abstract_options_provider {
 public:
  explicit Mysql_object_reader_options(
      const Mysql_chain_element_options *mysql_chain_element_options);

  void create_options() override;

  uint64 m_row_group_size;
  const Mysql_chain_element_options *m_mysql_chain_element_options;
  bool m_skip_gipk;
};
}  // namespace Dump
}  // namespace Tools
}  // namespace Mysql

#endif

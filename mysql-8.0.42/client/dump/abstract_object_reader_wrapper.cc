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

#include "client/dump/abstract_object_reader_wrapper.h"

#include <functional>

using namespace Mysql::Tools::Dump;

void Abstract_object_reader_wrapper::format_object(
    Item_processing_data *current_processing_data) {
  for (std::vector<I_object_reader *>::iterator it = m_object_readers.begin();
       it != m_object_readers.end(); ++it) {
    (*it)->read_object(
        this->object_to_be_processed_in_child(current_processing_data, *it));
  }
}

Abstract_object_reader_wrapper::Abstract_object_reader_wrapper(
    std::function<bool(const Mysql::Tools::Base::Message_data &)>
        *message_handler,
    Simple_id_generator *object_id_generator)
    : Abstract_chain_element(message_handler, object_id_generator) {}

void Abstract_object_reader_wrapper::register_object_reader(
    I_object_reader *new_object_reader) {
  m_object_readers.push_back(new_object_reader);
}

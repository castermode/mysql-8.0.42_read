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

#ifndef THREAD_SPECIFIC_CONNECTION_PROVIDER_INCLUDED
#define THREAD_SPECIFIC_CONNECTION_PROVIDER_INCLUDED

#include <functional>
#include <mutex>
#include <thread>
#include <unordered_map>
#include <vector>

#include "client/dump/abstract_connection_provider.h"

namespace Mysql {
namespace Tools {
namespace Dump {

class Thread_specific_connection_provider
    : public Abstract_connection_provider {
 public:
  explicit Thread_specific_connection_provider(
      Mysql::Tools::Base::I_connection_factory *connection_factory);
  ~Thread_specific_connection_provider() override;

  Mysql::Tools::Base::Mysql_query_runner *get_runner(
      std::function<bool(const Mysql::Tools::Base::Message_data &)>
          *message_handler) override;

 private:
  std::mutex mu;
  std::unordered_map<std::thread::id, Mysql::Tools::Base::Mysql_query_runner *>
      m_runners;
};

}  // namespace Dump
}  // namespace Tools
}  // namespace Mysql

#endif

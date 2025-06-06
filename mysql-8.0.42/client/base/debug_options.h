/*
   Copyright (c) 2014, 2025, Oracle and/or its affiliates.

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

#ifndef DEBUG_OPTIONS_INCLUDED
#define DEBUG_OPTIONS_INCLUDED

#include <optional>

#include "client/base/abstract_options_provider.h"
#include "my_compiler.h"

namespace Mysql {
namespace Tools {
namespace Base {

class Abstract_program;

namespace Options {

/**
  Options provider providing debugging options.
 */
class Debug_options : public Abstract_options_provider {
 public:
  /**
    Constructs new debug options provider.
    @param program Pointer to main program class.
   */
  explicit Debug_options(Abstract_program *program);
  /**
    Creates all options that will be provided.
    Implementation of Abstract_options_provider virtual method.
   */
  void create_options() override;
  /**
    Callback to be called when command-line options parsing have finished.
  */
  void options_parsed() override;

 private:
  void debug_option_callback(char *argument [[maybe_unused]]);

  Abstract_program *m_program;
  bool m_debug_info_flag{false};
  bool m_debug_check_flag{false};
  std::optional<std::string> m_dbug_option;
};

}  // namespace Options
}  // namespace Base
}  // namespace Tools
}  // namespace Mysql

#endif

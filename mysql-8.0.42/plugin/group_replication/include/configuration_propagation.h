/* Copyright (c) 2021, 2025, Oracle and/or its affiliates.

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

#ifndef CONFIGURATION_PROPAGATION_INCLUDE
#define CONFIGURATION_PROPAGATION_INCLUDE

/**
  @class Configuration_propagation

  Interface for configuration propagation through
  Member_actions_handler.
*/
class Configuration_propagation {
 public:
  virtual ~Configuration_propagation() {}

  /**
    Propagate the serialized configuration to the group.

    @param[in] serialized_configuration
                            the serialized configuration

    @return the operation status
      @retval false  Successful
      @retval true   Error
  */
  virtual bool propagate_serialized_configuration(
      const std::string &serialized_configuration) = 0;
};

#endif /* CONFIGURATION_PROPAGATION_INCLUDE */

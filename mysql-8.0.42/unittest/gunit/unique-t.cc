/* Copyright (c) 2013, 2025, Oracle and/or its affiliates.

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

#include <gtest/gtest.h>
#include <stddef.h>
#include <sys/types.h>

#include "sql/sql_class.h"
#include "sql/uniques.h"
#include "unittest/gunit/fake_costmodel.h"
#include "unittest/gunit/test_utils.h"

namespace unique_unittest {

using my_testing::Server_initializer;

class UniqueCostTest : public ::testing::Test {
 protected:
  void SetUp() override { initializer.SetUp(); }
  void TearDown() override { initializer.TearDown(); }

  THD *thd() { return initializer.thd(); }

  Server_initializer initializer;
};

// This is an excerpt of code from get_best_disjunct_quick()
TEST_F(UniqueCostTest, GetUseCost) {
  const ulong num_keys = 328238;
  const ulong key_size = 96;

  // Set up the optimizer cost model
  Fake_Cost_model_table cost_model_table;

  const double dup_removal_cost = Unique::get_use_cost(
      num_keys, key_size, MIN_SORT_MEMORY, &cost_model_table);
  EXPECT_GT(dup_removal_cost, 0.0);
}

}  // namespace unique_unittest

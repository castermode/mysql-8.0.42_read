/*
 * Copyright (c) 2018, 2025, Oracle and/or its affiliates.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License, version 2.0,
 * as published by the Free Software Foundation.
 *
 * This program is designed to work with certain software (including
 * but not limited to OpenSSL) that is licensed under separate terms,
 * as designated in a particular file or component or in included license
 * documentation.  The authors of MySQL hereby grant you an additional
 * permission to link the program and your derivative works with the
 * separately licensed software that they have either included with
 * the program or referenced in the documentation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License, version 2.0, for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301  USA
 */

#ifndef PLUGIN_X_SRC_HELPER_MULTITHREAD_COND_H_
#define PLUGIN_X_SRC_HELPER_MULTITHREAD_COND_H_

#include "mysql/psi/mysql_cond.h"
#include "plugin/x/src/helper/multithread/mutex.h"
#include "thr_cond.h"

namespace xpl {

class Cond {
 public:
  Cond(PSI_cond_key key);
  ~Cond();

  void wait(Mutex &mutex);
  int timed_wait(Mutex &mutex, unsigned long long nanoseconds);
  void signal();
  void signal(Mutex &mutex);
  void broadcast();
  void broadcast(Mutex &mutex);

 private:
  Cond(const Cond &) = delete;
  Cond &operator=(const Cond &) = delete;

  mysql_cond_t m_cond;
};

}  // namespace xpl

#endif  // PLUGIN_X_SRC_HELPER_MULTITHREAD_COND_H_

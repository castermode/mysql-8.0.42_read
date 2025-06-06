/* Copyright (c) 2017, 2025, Oracle and/or its affiliates.

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

#ifndef COMPONENTS_SERVICES_PSI_RWLOCK_SERVICE_H
#define COMPONENTS_SERVICES_PSI_RWLOCK_SERVICE_H

#include <mysql/components/service.h>
#include <mysql/components/services/bits/psi_rwlock_bits.h>

#if 0
/*
  Version 1.
  Introduced in MySQL 8.0.3
  Obsoleted in MySQL 8.0.17
  Status: Obsolete, use version 2 instead.
*/
BEGIN_SERVICE_DEFINITION(psi_rwlock_v1)
/** @sa register_rwlock_v1_t. */
register_rwlock_v1_t register_rwlock;
/** @sa init_rwlock_v1_t. */
init_rwlock_v1_t init_rwlock;
/** @sa destroy_rwlock_v1_t. */
destroy_rwlock_v1_t destroy_rwlock;
/** @sa start_rwlock_rdwait_v1_t. */
start_rwlock_rdwait_v1_t start_rwlock_rdwait;
/** @sa end_rwlock_rdwait_v1_t. */
end_rwlock_rdwait_v1_t end_rwlock_rdwait;
/** @sa start_rwlock_wrwait_v1_t. */
start_rwlock_wrwait_v1_t start_rwlock_wrwait;
/** @sa end_rwlock_wrwait_v1_t. */
end_rwlock_wrwait_v1_t end_rwlock_wrwait;
/** @sa unlock_rwlock_v1_t. */
unlock_rwlock_v1_t unlock_rwlock;
END_SERVICE_DEFINITION(psi_rwlock_v1)
#endif

/*
  Version 2.
  Introduced in MySQL 8.0.17
  Status: active
*/
BEGIN_SERVICE_DEFINITION(psi_rwlock_v2)
/** @sa register_rwlock_v1_t. */
register_rwlock_v1_t register_rwlock;
/** @sa init_rwlock_v1_t. */
init_rwlock_v1_t init_rwlock;
/** @sa destroy_rwlock_v1_t. */
destroy_rwlock_v1_t destroy_rwlock;
/** @sa start_rwlock_rdwait_v1_t. */
start_rwlock_rdwait_v1_t start_rwlock_rdwait;
/** @sa end_rwlock_rdwait_v1_t. */
end_rwlock_rdwait_v1_t end_rwlock_rdwait;
/** @sa start_rwlock_wrwait_v1_t. */
start_rwlock_wrwait_v1_t start_rwlock_wrwait;
/** @sa end_rwlock_wrwait_v1_t. */
end_rwlock_wrwait_v1_t end_rwlock_wrwait;
/** @sa unlock_rwlock_v2_t. */
unlock_rwlock_v2_t unlock_rwlock;
END_SERVICE_DEFINITION(psi_rwlock_v2)

#endif /* COMPONENTS_SERVICES_PSI_RWLOCK_SERVICE_H */

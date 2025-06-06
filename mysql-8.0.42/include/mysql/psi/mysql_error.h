/* Copyright (c) 2016, 2025, Oracle and/or its affiliates.

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

#ifndef MYSQL_ERROR_H
#define MYSQL_ERROR_H

/**
  @file include/mysql/psi/mysql_error.h
  Instrumentation helpers for errors.
*/

/* HAVE_PSI_*_INTERFACE */
#include "my_psi_config.h"  // IWYU pragma: keep

#include "mysql/psi/psi_error.h"

#if defined(MYSQL_SERVER) || defined(PFS_DIRECT_CALL)
/* PSI_ERROR_CALL() as direct call. */
#include "pfs_error_provider.h"  // IWYU pragma: keep
#endif

#ifndef PSI_ERROR_CALL
#define PSI_ERROR_CALL(M) psi_error_service->M
#endif

/**
  @defgroup psi_api_error Error instrumentation (API)
  @ingroup psi_api
  @{
*/

/**
  @def MYSQL_LOG_ERROR(E, N)
  Instrumented metadata lock destruction.
  @param N Error number
  @param T Error operation
*/
#ifdef HAVE_PSI_ERROR_INTERFACE
#define MYSQL_LOG_ERROR(N, T) inline_mysql_log_error(N, T)
#else
#define MYSQL_LOG_ERROR(N, T) \
  do {                        \
  } while (0)
#endif

#ifdef HAVE_PSI_ERROR_INTERFACE

static inline void inline_mysql_log_error(int error_num,
                                          PSI_error_operation error_operation) {
  PSI_ERROR_CALL(log_error)(error_num, error_operation);
}
#endif /* HAVE_PSI_ERROR_INTERFACE */

/** @} (end of group psi_api_error) */

#endif

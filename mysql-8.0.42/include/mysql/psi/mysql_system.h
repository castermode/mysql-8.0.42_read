/* Copyright (c) 2018, 2025, Oracle and/or its affiliates.

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

#ifndef MYSQL_SYSTEM_H
#define MYSQL_SYSTEM_H

/**
  @file include/mysql/psi/mysql_system.h
  Instrumentation helpers for system-level events.
*/

#include "my_compiler.h"

/* HAVE_PSI_*_INTERFACE */
#include "my_psi_config.h"  // IWYU pragma: keep

#include "mysql/psi/psi_system.h"

#if defined(MYSQL_SERVER) || defined(PFS_DIRECT_CALL)
/* PSI_SYSTEM_CALL() as direct call. */
#include "pfs_system_provider.h"  // IWYU pragma: keep
#endif

#ifndef PSI_SYSTEM_CALL
#define PSI_SYSTEM_CALL(M) psi_system_service->M
#endif

/**
  @defgroup psi_api_system Thread Instrumentation (API)
  @ingroup psi_api
  @{
*/

/**
  @def mysql_unload_plugin(P1)
  Psi system unload plugin event.
*/
#define mysql_unload_plugin(P1) inline_mysql_unload_plugin(P1)

static inline void inline_mysql_unload_plugin(const char *plugin_name
                                              [[maybe_unused]]) {
#ifdef HAVE_PSI_SYSTEM_INTERFACE
  PSI_SYSTEM_CALL(unload_plugin)(plugin_name);
#endif
}

/** @} (end of group psi_api_system) */

#endif

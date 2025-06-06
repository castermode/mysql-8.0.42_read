/* Copyright (c) 2008, 2025, Oracle and/or its affiliates.

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

#ifndef MYSQL_PSI_STATEMENT_H
#define MYSQL_PSI_STATEMENT_H

/**
  @file include/mysql/psi/psi_statement.h
  Performance schema instrumentation interface.

  @defgroup psi_abi_statement Statement Instrumentation (ABI)
  @ingroup psi_abi
  @{
*/

#include "my_inttypes.h"
#include "my_macros.h"

/* HAVE_PSI_*_INTERFACE */
#include "my_psi_config.h"  // IWYU pragma: keep

#include "my_sharedlib.h"
#include "mysql/components/services/bits/psi_statement_bits.h"

/** Entry point for the performance schema interface. */
struct PSI_statement_bootstrap {
  /**
    ABI interface finder.
    Calling this method with an interface version number returns either
    an instance of the ABI for this version, or NULL.
    @sa PSI_STATEMENT_VERSION_1
    @sa PSI_STATEMENT_VERSION_2
    @sa PSI_STATEMENT_VERSION_3
    @sa PSI_STATEMENT_VERSION_4
    @sa PSI_CURRENT_STATEMENT_VERSION
  */
  void *(*get_interface)(int version);
};
typedef struct PSI_statement_bootstrap PSI_statement_bootstrap;

#ifdef HAVE_PSI_STATEMENT_INTERFACE

/**
  Performance Schema Statement Interface, version 1.
  @since PSI_STATEMENT_VERSION_1
*/
struct PSI_statement_service_v1 {
  /* No binary compatibility with old PLUGIN */
  void *this_interface_is_obsolete;
};

/**
  Performance Schema Statement Interface, version 2.
  @since PSI_STATEMENT_VERSION_2
*/
struct PSI_statement_service_v2 {
  /* No binary compatibility with old PLUGIN */
  void *this_interface_is_obsolete;
};

/**
  Performance Schema Statement Interface, version 3.
  @since PSI_STATEMENT_VERSION_3
*/
struct PSI_statement_service_v3 {
  /* No binary compatibility with old PLUGIN */
  void *this_interface_is_obsolete;
};

/**
  Performance Schema Statement Interface, version 4.
  @since PSI_STATEMENT_VERSION_4
*/
struct PSI_statement_service_v4 {
  /* No binary compatibility with old PLUGIN */
  void *this_interface_is_obsolete;
};

/**
  Performance Schema Statement Interface, version 5.
  @since PSI_STATEMENT_VERSION_5
*/
struct PSI_statement_service_v5 {
  /** @sa register_statement_v1_t. */
  register_statement_v1_t register_statement;
  /** @sa get_thread_statement_locker_v5_t. */
  get_thread_statement_locker_v5_t get_thread_statement_locker;
  /** @sa refine_statement_v1_t. */
  refine_statement_v1_t refine_statement;
  /** @sa start_statement_v1_t. */
  start_statement_v1_t start_statement;
  /** @sa set_statement_text_v1_t. */
  set_statement_text_v1_t set_statement_text;
  /** @sa set_statement_query_id. */
  set_statement_query_id_t set_statement_query_id;
  /** @sa set_statement_lock_time_t. */
  set_statement_lock_time_t set_statement_lock_time;
  /** @sa set_statement_rows_sent_t. */
  set_statement_rows_sent_t set_statement_rows_sent;
  /** @sa set_statement_rows_examined_t. */
  set_statement_rows_examined_t set_statement_rows_examined;
  /** @sa inc_statement_created_tmp_disk_tables. */
  inc_statement_created_tmp_disk_tables_t inc_statement_created_tmp_disk_tables;
  /** @sa inc_statement_created_tmp_tables. */
  inc_statement_created_tmp_tables_t inc_statement_created_tmp_tables;
  /** @sa inc_statement_select_full_join. */
  inc_statement_select_full_join_t inc_statement_select_full_join;
  /** @sa inc_statement_select_full_range_join. */
  inc_statement_select_full_range_join_t inc_statement_select_full_range_join;
  /** @sa inc_statement_select_range. */
  inc_statement_select_range_t inc_statement_select_range;
  /** @sa inc_statement_select_range_check. */
  inc_statement_select_range_check_t inc_statement_select_range_check;
  /** @sa inc_statement_select_scan. */
  inc_statement_select_scan_t inc_statement_select_scan;
  /** @sa inc_statement_sort_merge_passes. */
  inc_statement_sort_merge_passes_t inc_statement_sort_merge_passes;
  /** @sa inc_statement_sort_range. */
  inc_statement_sort_range_t inc_statement_sort_range;
  /** @sa inc_statement_sort_rows. */
  inc_statement_sort_rows_t inc_statement_sort_rows;
  /** @sa inc_statement_sort_scan. */
  inc_statement_sort_scan_t inc_statement_sort_scan;
  /** @sa set_statement_no_index_used. */
  set_statement_no_index_used_t set_statement_no_index_used;
  /** @sa set_statement_no_good_index_used. */
  set_statement_no_good_index_used_t set_statement_no_good_index_used;
  /** @sa set_statement_secondary_engine_v3_t. */
  set_statement_secondary_engine_v3_t set_statement_secondary_engine;
  /** @sa end_statement_v1_t. */
  end_statement_v1_t end_statement;

  /** @sa create_prepared_stmt_v1_t. */
  create_prepared_stmt_v1_t create_prepared_stmt;
  /** @sa destroy_prepared_stmt_v1_t. */
  destroy_prepared_stmt_v1_t destroy_prepared_stmt;
  /** @sa reprepare_prepared_stmt_v1_t. */
  reprepare_prepared_stmt_v1_t reprepare_prepared_stmt;
  /** @sa execute_prepared_stmt_v1_t. */
  execute_prepared_stmt_v1_t execute_prepared_stmt;
  /** @sa set_prepared_stmt_text_v1_t. */
  set_prepared_stmt_text_v1_t set_prepared_stmt_text;
  /** @sa set_prepared_stmt_secondary_engine_v3_t */
  set_prepared_stmt_secondary_engine_v3_t set_prepared_stmt_secondary_engine;

  /** @sa digest_start_v1_t. */
  digest_start_v1_t digest_start;
  /** @sa digest_end_v1_t. */
  digest_end_v1_t digest_end;

  /** @sa get_sp_share_v1_t. */
  get_sp_share_v1_t get_sp_share;
  /** @sa release_sp_share_v1_t. */
  release_sp_share_v1_t release_sp_share;
  /** @sa start_sp_v1_t. */
  start_sp_v1_t start_sp;
  /** @sa start_sp_v1_t. */
  end_sp_v1_t end_sp;
  /** @sa drop_sp_v1_t. */
  drop_sp_v1_t drop_sp;

  notify_statement_query_attributes_v5_t notify_statement_query_attributes;
  statement_abort_telemetry_v5_t statement_abort_telemetry;
};

typedef struct PSI_statement_service_v5 PSI_statement_service_t;

extern MYSQL_PLUGIN_IMPORT PSI_statement_service_t *psi_statement_service;

#endif /* HAVE_PSI_STATEMENT_INTERFACE */

/** @} (end of group psi_abi_statement) */

#endif /* MYSQL_PSI_STATEMENT_H */

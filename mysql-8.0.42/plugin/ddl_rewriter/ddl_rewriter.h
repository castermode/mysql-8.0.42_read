/*  Copyright (c) 2018, 2025, Oracle and/or its affiliates.

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

#ifndef PLUGIN_DDL_REWRITER_INCLUDED
#define PLUGIN_DDL_REWRITER_INCLUDED

#include <string>

/**
  Rewrite CREATE TABLE queries.

  Remove ENCRYPTION and DATA|INDEX DIRECTORY clauses. Note that the
  CREATE keyword is expected to be located at the very beginning of
  the query string. The function is used by the ddl_rewriter plugin,
  and is intended used for CREATE TABLE queries generated by
  SHOW CREATE TABLE, e.g. when restoring output from mysqldump.

  @param       query            Original query.
  @param [out] rewritten_query  Rewritten query with clauses removed. If
                                there was no rewrite, rewritten_query will
                                not be modified.

  @retval true if the query was rewritten

*/

bool query_rewritten(const std::string &query, std::string *rewritten_query);

#endif  // PLUGIN_DDL_REWRITER_INCLUDED

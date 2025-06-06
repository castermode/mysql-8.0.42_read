/* Copyright (c) 2022, 2025, Oracle and/or its affiliates.

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

#ifndef XA_SQL_CMD_XA_PREPARE
#define XA_SQL_CMD_XA_PREPARE

#include "sql/sql_cmd.h"  // Sql_cmd
#include "sql/xa.h"       // xid_t

/**
  @class Sql_cmd_xa_prepare

  This class represents the `XA PREPARE ...` SQL statement which puts in the
  PREPARED state an XA transaction with the given xid value.

  @see Sql_cmd
*/
class Sql_cmd_xa_prepare : public Sql_cmd {
 public:
  /**
    Class constructor.

    @param xid_arg XID of the XA transacation about to be prepared
   */
  explicit Sql_cmd_xa_prepare(xid_t *xid_arg);
  virtual ~Sql_cmd_xa_prepare() override = default;

  /**
    Retrieves the SQL command code for this class, `SQLCOM_XA_PREPARE`.

    @see Sql_cmd::sql_command_code

    @return The SQL command code for this class, `SQLCOM_XA_PREPARE`.
   */
  enum_sql_command sql_command_code() const override;
  /**
    Executes the SQL command.

    @see Sql_cmd::execute

    @param thd The `THD` session object within which the command is being
               executed.

    @return false if the execution is successful, true otherwise.
   */
  bool execute(THD *thd) override;

 private:
  /** The XID associated with the underlying XA transaction. */
  xid_t *m_xid;

  /**
    Puts an XA transaction in the PREPARED state.

    @param thd The `THD` session object within which the command is being
               executed.

    @retval false  Success
    @retval true   Failure
  */
  bool trans_xa_prepare(THD *thd);
};

#endif  // XA_SQL_CMD_XA_PREPARE

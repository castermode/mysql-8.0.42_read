/* Copyright (c) 2020, 2025, Oracle and/or its affiliates.

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

#include "xcom/xdr_checked_data.h"

/* Encode and decode a application data with added check that there is enough
 * data when decoding */
bool_t xdr_checked_data(XDR *xdrs, checked_data *objp) {
  /*
          Sanity check. x_handy is number of remaining bytes. For old XDR,
          x_handy is int type. So type cast is used to eliminate a warning.
  */
  if (xdrs->x_op == XDR_DECODE && (objp->data_len + 4) > (u_int)xdrs->x_handy)
    return FALSE;
  return xdr_bytes(xdrs, (char **)&objp->data_val, (u_int *)&objp->data_len,
                   0xffffffff);
}

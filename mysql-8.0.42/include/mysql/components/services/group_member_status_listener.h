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

#ifndef GROUP_MEMBER_STATUS_LISTENER_H
#define GROUP_MEMBER_STATUS_LISTENER_H

#include <mysql/components/service.h>

/**
  A service that listens for notifications about member state
  or member role updates.
*/
BEGIN_SERVICE_DEFINITION(group_member_status_listener)
/**
  This function SHALL be called whenever the role of a member
  changes.

  The implementation SHALL consume the notification and
  return false on success, true on failure.

  The implementation MUST NOT block the caller. It MUST
  handle the notification quickly or enqueue it and deal
  with it asynchronously.

  Currently the server can have one of two roles in group
  replication, when setup in single primary mode: PRIMARY
  or SECONDARY.

  @param view_id The view identifier. This must be copied
                 if the string must outlive the notification
                 lifecycle.

  @return false success, true on failure.
*/
DECLARE_BOOL_METHOD(notify_member_role_change, (const char *view_id));

/**
  This function SHALL be called whenever the state of a member
  changes.

  The implementation SHALL consume the notification and
  return false on success, true on failure.

  The implementation MUST NOT block the caller. It MUST
  handle the notification quickly or enqueue it and deal
  with it asynchronously.

  A state is one of OFFLINE, ONLINE, RECOVERING, UNREACHABLE,
  ERROR.

  @param view_id The view identifier. This must be copied
                 if the string must outlive the notification
                 lifecycle.

  @return false success, true on failure.
*/
DECLARE_BOOL_METHOD(notify_member_state_change, (const char *view_id));
END_SERVICE_DEFINITION(group_member_status_listener)

#endif /* GROUP_MEMBER_STATUS_LISTENER_H */

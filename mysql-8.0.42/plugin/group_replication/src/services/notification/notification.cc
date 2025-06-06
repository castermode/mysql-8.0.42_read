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

#include <mysql/components/services/group_member_status_listener.h>
#include <mysql/components/services/group_membership_listener.h>
#include <mysql/components/services/log_builtins.h>

#include "plugin/group_replication/include/plugin.h"
#include "plugin/group_replication/include/services/notification/notification.h"

#include <list>

enum SvcTypes { kGroupMembership = 0, kGroupMemberStatus };

typedef int (*svc_notify_func)(Notification_context &, my_h_service);

static int notify_group_membership(Notification_context &ctx,
                                   my_h_service svc) {
  int svc_ko = 0;
  const char *view_id = ctx.get_view_id().c_str();
  SERVICE_TYPE(group_membership_listener) *listener = nullptr;

  /* now that we have the handler for it, notify */
  listener = reinterpret_cast<SERVICE_TYPE(group_membership_listener) *>(svc);

  if (ctx.get_view_changed()) {
    svc_ko = svc_ko + listener->notify_view_change(view_id);
  }

  if (ctx.get_quorum_lost()) {
    svc_ko = svc_ko + listener->notify_quorum_loss(view_id);
  }

  return svc_ko;
}

static int notify_group_member_status(Notification_context &ctx,
                                      my_h_service svc) {
  int svc_ko = 0;
  const char *view_id = ctx.get_view_id().c_str();
  SERVICE_TYPE(group_member_status_listener) *listener = nullptr;

  /* now that we have the handler for it, notify */
  listener =
      reinterpret_cast<SERVICE_TYPE(group_member_status_listener) *>(svc);

  if (ctx.get_member_state_changed()) {
    svc_ko = svc_ko + listener->notify_member_state_change(view_id);
  }

  if (ctx.get_member_role_changed()) {
    svc_ko = svc_ko + listener->notify_member_role_change(view_id);
  }

  return svc_ko;
}

/**
  Auxiliary function to engage the service registry to
  notify a set of listeners.

  @param svc_type The service name.
  @param ctx The events context

  @return false on success, true otherwise.
 */
static bool notify(SvcTypes svc_type, Notification_context &ctx) {
  SERVICE_TYPE(registry) *r = nullptr;
  SERVICE_TYPE(registry_query) *rq = nullptr;
  my_h_service_iterator h_ret_it = nullptr;
  my_h_service h_listener_svc = nullptr;
  bool res = false;
  bool is_service_default_implementation = true;
  std::string svc_name;
  svc_notify_func notify_func_ptr;
  std::list<std::string> listeners_names;

  if (!registry_module || !(r = registry_module->get_registry_handle()) ||
      !(rq = registry_module->get_registry_query_handle()))
    return true;

  /*
    Decides which listener service to notify, based on the
    service type. It also checks whether the service should
    be notified indeed, based on the event context.

    If the event is not to be notified, the function returns
    immediately.
   */
  switch (svc_type) {
    case kGroupMembership:
      notify_func_ptr = notify_group_membership;
      svc_name = Registry_module_interface::SVC_NAME_MEMBERSHIP;
      break;
    case kGroupMemberStatus:
      notify_func_ptr = notify_group_member_status;
      svc_name = Registry_module_interface::SVC_NAME_STATUS;
      break;
    default:
      assert(false); /* purecov: inspected */
      /* production builds default to membership */
      svc_name = Registry_module_interface::SVC_NAME_MEMBERSHIP;
      notify_func_ptr = notify_group_membership;
      break;
  }

  /*
    create iterator to navigate notification GMS change
    notification listeners
  */
  if (rq->create(svc_name.c_str(), &h_ret_it)) {
    /* no listener registered, skip */
    if (h_ret_it) {
      rq->release(h_ret_it);
    }
    return false;
  }

  /*
    To avoid acquire multiple re-entrant locks on the services
    registry, which would happen by calling registry_module::acquire()
    after calling registry_module::create(), we store the services names
    and only notify them after release the iterator.
  */
  for (; h_ret_it != nullptr && !rq->is_valid(h_ret_it); rq->next(h_ret_it)) {
    const char *next_svc_name = nullptr;

    /* get next registered listener */
    if (rq->get(h_ret_it, &next_svc_name)) {
      res |= true;
      continue;
    }

    /*
      The iterator currently contains more service implementations than
      those named after the given service name. The spec says that the
      name given is used to position the iterator start on the first
      registered service implementation prefixed with that name. We need
      to iterate until the next element in the iterator (service implementation)
      has a different service name.
    */
    std::string s(next_svc_name);
    if (s.find(svc_name, 0) == std::string::npos) break;

    /*
      The iterator currently contains more service implementations than
      those named after the given service name, the first registered
      service will be listed twice: 1) default service, 2) regular service.
      The spec says that the name given is used to position the iterator
      start on the first registered service implementation prefixed with
      that name. We need to skip the first service since it will be listed
      twice.
    */
    if (is_service_default_implementation) {
      is_service_default_implementation = false;
      continue;
    }

    listeners_names.push_back(s);
  }

  /* release the iterator */
  if (h_ret_it) rq->release(h_ret_it);

  /* notify all listeners */
  for (std::string listener_name : listeners_names) {
    /* acquire listener */
    if (!r->acquire(listener_name.c_str(), &h_listener_svc)) {
      if (notify_func_ptr(ctx, h_listener_svc)) {
        LogPluginErr(WARNING_LEVEL,
                     ER_GRP_RPL_FAILED_TO_NOTIFY_GRP_MEMBERSHIP_EVENT,
                     listener_name.c_str());
      }
    }

    /* release the listener service */
    r->release(h_listener_svc);
  }

  return res;
}

/* Public Functions */

bool notify_and_reset_ctx(Notification_context &ctx) {
  bool res = false;

  if (ctx.get_view_changed() || ctx.get_quorum_lost()) {
    /* notify membership events listeners. */
    if (notify(kGroupMembership, ctx)) {
      /* purecov: begin inspected */
      LogPluginErr(ERROR_LEVEL,
                   ER_GRP_RPL_FAILED_TO_BROADCAST_GRP_MEMBERSHIP_NOTIFICATION);
      /* purecov: end */
      res = true; /* purecov: inspected */
    }
  }

  if (ctx.get_member_state_changed() || ctx.get_member_role_changed()) {
    /* notify member status events listeners. */
    if (notify(kGroupMemberStatus, ctx)) {
      /* purecov: begin inspected */
      LogPluginErr(ERROR_LEVEL,
                   ER_GRP_RPL_FAILED_TO_BROADCAST_MEMBER_STATUS_NOTIFICATION);
      /* purecov: end */
      res = true; /* purecov: inspected */
    }
  }

  ctx.reset();
  return res;
}

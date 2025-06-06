/* Copyright (c) 2021, 2025, Oracle and/or its affiliates.

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

#ifndef AUTH_KERBEROS_CORE_H_
#define AUTH_KERBEROS_CORE_H_

#include <krb5/krb5.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "log_client.h"

/*
  Kerberos class is built around kerberos library.
  This class should/can be used for different part of code as standalone
  class.
  This class performs following operations:
  1. Authentication with kerberos server and store the credentials in cache.
  2. Get the default configured kerberos user in the OS, from default principal.

  Credentials:
  A ticket plus the secret session key necessary to use that ticket successfully
  in an authentication exchange.

  Principal:
  A named client or server entity that participates in a network communication,
  with one name that is considered canonical

  Credential cache:
  A credential cache (or �ccache�) holds Kerberos credentials while they remain
  valid and, generally, while the user�s session lasts, so that authenticating
  to a service multiple times (e.g., connecting to a web or mail server more
  than once) doesn�t require contacting the KDC every time.
*/
namespace auth_kerberos_context {
class Kerberos {
 public:
  Kerberos(const char *user, const char *password);
  ~Kerberos();
  /*
    1. This function authenticates with kerberos server.
    2. If TGT destroy is false, this function stores the TGT in Kerberos cache
    for subsequent usage.
    3. If user credentials already exist in the cache, it doesn't attempt to get
    it again.

    @return
      @retval true Successfully able to obtain and store credentials.
      @retval false Failed to obtain and store credentials.
  */
  bool obtain_store_credentials();
  /*
    This function retrieves default principle from kerberos configuration and
    parses the user name from it. If user name has not been provided in the
    MySQL client, This method can be used to get the user name  and use for
    authentication.
    @return
      @retval true Successfully able to get user name.
      @retval false Failed to get user name.
  */
  bool get_upn(std::string *name);
  void destroy_credentials();

 private:
  /*
    This function creates kerberos context, initializes credentials cache and
    user principal.
    @return
      @retval true All the required kerberos objects like context,
    credentials cache and user principal are initialized correctly.
      @retval false Required kerberos objects failed to initialized.
  */
  bool setup();
  /*
    This function frees kerberos context, credentials, credentials cache and
    user principal.
  */
  void cleanup();
  void log(int error_code);
  krb5_error_code store_credentials();
  krb5_error_code obtain_credentials();
  bool credential_valid();
  bool get_kerberos_config();

  bool m_initialized;
  std::string m_upn;
  std::string m_password;
  int m_destroy_tickets;
  krb5_context m_context;
  krb5_ccache m_krb_credentials_cache;
  krb5_creds m_credentials;
  bool m_credentials_created;
};
}  // namespace auth_kerberos_context
#endif  // AUTH_KERBEROS_CORE_H_

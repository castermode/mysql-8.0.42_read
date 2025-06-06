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

#include "sql/basic_ostream.h"
#include "my_inttypes.h"
#include "mysql/components/services/log_shared.h"
#include "mysql/psi/mysql_file.h"
#include "mysqld_error.h"
#include "sql/log.h"

IO_CACHE_ostream::IO_CACHE_ostream() = default;
IO_CACHE_ostream::~IO_CACHE_ostream() { close(); }

bool IO_CACHE_ostream::open(
#ifdef HAVE_PSI_INTERFACE
    PSI_file_key log_file_key [[maybe_unused]],
#endif
    const char *file_name, myf flags) {
  File file = -1;

  if ((file = mysql_file_open(log_file_key, file_name, O_CREAT | O_WRONLY,
                              MYF(MY_WME))) < 0)
    return true;

  if (init_io_cache(&m_io_cache, file, IO_SIZE, WRITE_CACHE, 0, false, flags)) {
    mysql_file_close(file, MYF(0));
    return true;
  }
  return false;
}

bool IO_CACHE_ostream::close() {
  if (my_b_inited(&m_io_cache)) {
    int ret = end_io_cache(&m_io_cache);
    ret |= mysql_file_close(m_io_cache.file, MYF(MY_WME));
    return ret != 0;
  }
  return false;
}

bool IO_CACHE_ostream::seek(my_off_t offset) {
  assert(my_b_inited(&m_io_cache));
  return reinit_io_cache(&m_io_cache, WRITE_CACHE, offset, false, true);
}

bool IO_CACHE_ostream::write(const unsigned char *buffer, my_off_t length) {
  assert(my_b_inited(&m_io_cache));
  DBUG_EXECUTE_IF("simulate_ostream_write_failure", return true;);
  return my_b_safe_write(&m_io_cache, buffer, length);
}

bool IO_CACHE_ostream::truncate(my_off_t offset) {
  assert(my_b_inited(&m_io_cache));
  assert(m_io_cache.file != -1);

  if (my_chsize(m_io_cache.file, offset, 0, MYF(MY_WME))) return true;

  reinit_io_cache(&m_io_cache, WRITE_CACHE, offset, false, true);
  return false;
}

bool IO_CACHE_ostream::flush() {
  assert(my_b_inited(&m_io_cache));
  return flush_io_cache(&m_io_cache);
}

bool IO_CACHE_ostream::sync() {
  assert(my_b_inited(&m_io_cache));
  return mysql_file_sync(m_io_cache.file, MYF(MY_WME)) != 0;
}

bool Compressed_ostream::write(const unsigned char *buffer, my_off_t length) {
  DBUG_TRACE;
  m_compressor->feed(buffer, length);
  m_status = m_compressor->compress(m_managed_buffer_sequence);
  return m_status != Status_t::success;
}

Compressed_ostream::Status_t Compressed_ostream::get_status() const {
  return m_status;
}

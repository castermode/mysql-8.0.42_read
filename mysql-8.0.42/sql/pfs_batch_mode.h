#ifndef PFS_BATCH_MODE_INCLUDED
#define PFS_BATCH_MODE_INCLUDED

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

#include "sql/iterators/row_iterator.h"

/// A RAII class to handle turning on batch mode in front of scanning a row
/// iterator, and then turn it back off afterwards (on destruction).
///
/// Use this when scanning an iterator, e.g. using a for loop. See the comments
/// on RowIterator::StartPSIBatchMode for more details.
class PFSBatchMode {
 public:
  explicit PFSBatchMode(RowIterator *iterator) : m_iterator(iterator) {
    m_iterator->StartPSIBatchMode();
  }

  ~PFSBatchMode() { m_iterator->EndPSIBatchModeIfStarted(); }

 private:
  RowIterator *m_iterator;
};
#endif /* PFS_BATCH_MODE_INCLUDED */

#ifndef GIS__HAUSDORFF_DISTANCE_H_INCLUDED
#define GIS__HAUSDORFF_DISTANCE_H_INCLUDED

// Copyright (c) 2020, 2025, Oracle and/or its affiliates.
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License, version 2.0,
// as published by the Free Software Foundation.
//
// This program is designed to work with certain software (including
// but not limited to OpenSSL) that is licensed under separate terms,
// as designated in a particular file or component or in included license
// documentation.  The authors of MySQL hereby grant you an additional
// permission to link the program and your derivative works with the
// separately licensed software that they have either included with
// the program or referenced in the documentation.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License, version 2.0, for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301  USA.

/// @file
///
/// This file declares the interface to calculate the discrete Hausdorff
/// distance between linestrings.

#include "sql/dd/types/spatial_reference_system.h"  // dd::Spatial_reference_system
#include "sql/gis/geometries.h"

namespace gis {
/// Computes the discrete hausdorff distance between linestrings
///
/// @param[in] srs The spatial reference system, common to both geometries.
/// @param[in] g1 First geometry.
/// @param[in] g2 Second geometry.
/// @param[in] func_name Function name used in error reporting.
/// @param[out] hausdorff_distance The Hausdorff distance between g1 and g2 in
/// the SRS' linear unit.
/// @param[out] null True if the return value is NULL.
///
/// @retval false Success.
/// @retval true An error has occurred. The error has been reported with
/// my_error().
bool hausdorff_distance(const dd::Spatial_reference_system *srs,
                        const Geometry *g1, const Geometry *g2,
                        const char *func_name, double *hausdorff_distance,
                        bool *null) noexcept;

}  // namespace gis

#endif  // GIS__HAUSDORFF_DISTANCE_H_INCLUDED

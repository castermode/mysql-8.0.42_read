#ifndef GIS__FRECHET_DISTANCE_FUNCTOR_H_INCLUDED
#define GIS__FRECHET_DISTANCE_FUNCTOR_H_INCLUDED

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
/// This file declares the frechet_distance functor interface.
///
/// The functor is not intended for use directly by MySQL code. It should be
/// used indirectly through the gis::frechet_distance() function.
///
/// @see gis::frechet_distance

#include <memory>  // std::unique_ptr

#include <boost/geometry.hpp>
#include "sql/gis/functor.h"
#include "sql/gis/geometries.h"

namespace gis {

/// Frechet_distance functor that calls Boost.Geometry with the correct
/// parameter types.
///
/// The functor throws exceptions and is therefore only intended used to
/// implement frechet_distance or other geographic functions.
/// It should not be used directly by other MySQL code.
class Frechet_distance : public Functor<double> {
 private:
  std::unique_ptr<boost::geometry::strategy::distance::geographic<
      boost::geometry::strategy::andoyer,
      boost::geometry::srs::spheroid<double>>>
      m_geographic_strategy;

 public:
  Frechet_distance(double major, double minor);
  double operator()(const Geometry *g1, const Geometry *g2) const override;

  double eval(const Geometry *g1, const Geometry *g2) const;

  double eval(const Geographic_linestring *g1,
              const Geographic_linestring *g2) const;
  double eval(const Cartesian_linestring *g1,
              const Cartesian_linestring *g2) const;
};

}  // namespace gis

#endif  // GIS__FRECHET_DISTANCE_FUNCTOR_H_INCLUDED

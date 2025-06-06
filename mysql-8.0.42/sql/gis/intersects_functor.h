#ifndef SQL_GIS_INTERSECTS_FUNCTOR_H_INCLUDED
#define SQL_GIS_INTERSECTS_FUNCTOR_H_INCLUDED

// Copyright (c) 2017, 2025, Oracle and/or its affiliates.
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
/// This file declares the intersects functor interface.
///
/// The functor is not intended for use directly by MySQL code. It should be
/// used indirectly through the gis::intersects() function.
///
/// @see gis::intersects

#include <boost/geometry.hpp>

#include "sql/gis/box.h"
#include "sql/gis/functor.h"
#include "sql/gis/geometries.h"
#include "sql/gis/geometries_traits.h"

namespace gis {

/// Intersects functor that calls Boost.Geometry with the correct parameter
/// types.
///
/// The functor throws exceptions and is therefore only intended used to
/// implement intersects or other geographic functions. It should not be used
/// directly by other MySQL code.
class Intersects : public Functor<bool> {
 private:
  /// Semi-major axis of ellipsoid.
  double m_semi_major;
  /// Semi-minor axis of ellipsoid.
  double m_semi_minor;
  /// Strategy used for P/L and P/A.
  boost::geometry::strategy::within::geographic_winding<Geographic_point>
      m_geographic_pl_pa_strategy;
  /// Strategy used for L/L, L/A and A/A.
  boost::geometry::strategy::intersection::geographic_segments<>
      m_geographic_ll_la_aa_strategy;

 public:
  /// Creates a new Intersects functor.
  ///
  /// @param semi_major Semi-major axis of ellipsoid.
  /// @param semi_minor Semi-minor axis of ellipsoid.
  Intersects(double semi_major, double semi_minor);
  bool operator()(const Geometry *g1, const Geometry *g2) const override;
  bool operator()(const Box *b1, const Box *b2) const;
  bool eval(const Geometry *g1, const Geometry *g2) const;

  //////////////////////////////////////////////////////////////////////////////

  // intersects(Cartesian_point, *)

  bool eval(const Cartesian_point *g1, const Cartesian_point *g2) const;
  bool eval(const Cartesian_point *g1, const Cartesian_linestring *g2) const;
  bool eval(const Cartesian_point *g1, const Cartesian_polygon *g2) const;
  bool eval(const Cartesian_point *g1,
            const Cartesian_geometrycollection *g2) const;
  bool eval(const Cartesian_point *g1, const Cartesian_multipoint *g2) const;
  bool eval(const Cartesian_point *g1,
            const Cartesian_multilinestring *g2) const;
  bool eval(const Cartesian_point *g1, const Cartesian_multipolygon *g2) const;

  //////////////////////////////////////////////////////////////////////////////

  // intersects(Cartesian_linestring, *)

  bool eval(const Cartesian_linestring *g1, const Cartesian_point *g2) const;
  bool eval(const Cartesian_linestring *g1,
            const Cartesian_linestring *g2) const;
  bool eval(const Cartesian_linestring *g1, const Cartesian_polygon *g2) const;
  bool eval(const Cartesian_linestring *g1,
            const Cartesian_geometrycollection *g2) const;
  bool eval(const Cartesian_linestring *g1,
            const Cartesian_multipoint *g2) const;
  bool eval(const Cartesian_linestring *g1,
            const Cartesian_multilinestring *g2) const;
  bool eval(const Cartesian_linestring *g1,
            const Cartesian_multipolygon *g2) const;

  //////////////////////////////////////////////////////////////////////////////

  // intersects(Cartesian_polygon, *)

  bool eval(const Cartesian_polygon *g1, const Cartesian_point *g2) const;
  bool eval(const Cartesian_polygon *g1, const Cartesian_linestring *g2) const;
  bool eval(const Cartesian_polygon *g1, const Cartesian_polygon *g2) const;
  bool eval(const Cartesian_polygon *g1,
            const Cartesian_geometrycollection *g2) const;
  bool eval(const Cartesian_polygon *g1, const Cartesian_multipoint *g2) const;
  bool eval(const Cartesian_polygon *g1,
            const Cartesian_multilinestring *g2) const;
  bool eval(const Cartesian_polygon *g1,
            const Cartesian_multipolygon *g2) const;

  //////////////////////////////////////////////////////////////////////////////

  // intersects(Cartesian_geometrycollection, *)

  bool eval(const Cartesian_geometrycollection *g1, const Geometry *g2) const;

  //////////////////////////////////////////////////////////////////////////////

  // intersects(Cartesian_multipoint, *)

  bool eval(const Cartesian_multipoint *g1, const Cartesian_point *g2) const;
  bool eval(const Cartesian_multipoint *g1,
            const Cartesian_linestring *g2) const;
  bool eval(const Cartesian_multipoint *g1, const Cartesian_polygon *g2) const;
  bool eval(const Cartesian_multipoint *g1,
            const Cartesian_geometrycollection *g2) const;
  bool eval(const Cartesian_multipoint *g1,
            const Cartesian_multipoint *g2) const;
  bool eval(const Cartesian_multipoint *g1,
            const Cartesian_multilinestring *g2) const;
  bool eval(const Cartesian_multipoint *g1,
            const Cartesian_multipolygon *g2) const;

  //////////////////////////////////////////////////////////////////////////////

  // intersects(Cartesian_multilinestring, *)

  bool eval(const Cartesian_multilinestring *g1,
            const Cartesian_point *g2) const;
  bool eval(const Cartesian_multilinestring *g1,
            const Cartesian_linestring *g2) const;
  bool eval(const Cartesian_multilinestring *g1,
            const Cartesian_polygon *g2) const;
  bool eval(const Cartesian_multilinestring *g1,
            const Cartesian_geometrycollection *g2) const;
  bool eval(const Cartesian_multilinestring *g1,
            const Cartesian_multipoint *g2) const;
  bool eval(const Cartesian_multilinestring *g1,
            const Cartesian_multilinestring *g2) const;
  bool eval(const Cartesian_multilinestring *g1,
            const Cartesian_multipolygon *g2) const;

  //////////////////////////////////////////////////////////////////////////////

  // intersects(Cartesian_multipolygon, *)

  bool eval(const Cartesian_multipolygon *g1, const Cartesian_point *g2) const;
  bool eval(const Cartesian_multipolygon *g1,
            const Cartesian_linestring *g2) const;
  bool eval(const Cartesian_multipolygon *g1,
            const Cartesian_polygon *g2) const;
  bool eval(const Cartesian_multipolygon *g1,
            const Cartesian_geometrycollection *g2) const;
  bool eval(const Cartesian_multipolygon *g1,
            const Cartesian_multipoint *g2) const;
  bool eval(const Cartesian_multipolygon *g1,
            const Cartesian_multilinestring *g2) const;
  bool eval(const Cartesian_multipolygon *g1,
            const Cartesian_multipolygon *g2) const;

  //////////////////////////////////////////////////////////////////////////////

  // intersects(Geographic_point, *)

  bool eval(const Geographic_point *g1, const Geographic_point *g2) const;
  bool eval(const Geographic_point *g1, const Geographic_linestring *g2) const;
  bool eval(const Geographic_point *g1, const Geographic_polygon *g2) const;
  bool eval(const Geographic_point *g1,
            const Geographic_geometrycollection *g2) const;
  bool eval(const Geographic_point *g1, const Geographic_multipoint *g2) const;
  bool eval(const Geographic_point *g1,
            const Geographic_multilinestring *g2) const;
  bool eval(const Geographic_point *g1,
            const Geographic_multipolygon *g2) const;

  //////////////////////////////////////////////////////////////////////////////

  // intersects(Geographic_linestring, *)

  bool eval(const Geographic_linestring *g1, const Geographic_point *g2) const;
  bool eval(const Geographic_linestring *g1,
            const Geographic_linestring *g2) const;
  bool eval(const Geographic_linestring *g1,
            const Geographic_polygon *g2) const;
  bool eval(const Geographic_linestring *g1,
            const Geographic_geometrycollection *g2) const;
  bool eval(const Geographic_linestring *g1,
            const Geographic_multipoint *g2) const;
  bool eval(const Geographic_linestring *g1,
            const Geographic_multilinestring *g2) const;
  bool eval(const Geographic_linestring *g1,
            const Geographic_multipolygon *g2) const;

  //////////////////////////////////////////////////////////////////////////////

  // intersects(Geographic_polygon, *)

  bool eval(const Geographic_polygon *g1, const Geographic_point *g2) const;
  bool eval(const Geographic_polygon *g1,
            const Geographic_linestring *g2) const;
  bool eval(const Geographic_polygon *g1, const Geographic_polygon *g2) const;
  bool eval(const Geographic_polygon *g1,
            const Geographic_geometrycollection *g2) const;
  bool eval(const Geographic_polygon *g1,
            const Geographic_multipoint *g2) const;
  bool eval(const Geographic_polygon *g1,
            const Geographic_multilinestring *g2) const;
  bool eval(const Geographic_polygon *g1,
            const Geographic_multipolygon *g2) const;

  //////////////////////////////////////////////////////////////////////////////

  // intersects(Geographic_geometrycollection, *)

  bool eval(const Geographic_geometrycollection *g1, const Geometry *g2) const;

  //////////////////////////////////////////////////////////////////////////////

  // intersects(Geographic_multipoint, *)

  bool eval(const Geographic_multipoint *g1, const Geographic_point *g2) const;
  bool eval(const Geographic_multipoint *g1,
            const Geographic_linestring *g2) const;
  bool eval(const Geographic_multipoint *g1,
            const Geographic_polygon *g2) const;
  bool eval(const Geographic_multipoint *g1,
            const Geographic_geometrycollection *g2) const;
  bool eval(const Geographic_multipoint *g1,
            const Geographic_multipoint *g2) const;
  bool eval(const Geographic_multipoint *g1,
            const Geographic_multilinestring *g2) const;
  bool eval(const Geographic_multipoint *g1,
            const Geographic_multipolygon *g2) const;

  //////////////////////////////////////////////////////////////////////////////

  // intersects(Geographic_multilinestring, *)

  bool eval(const Geographic_multilinestring *g1,
            const Geographic_point *g2) const;
  bool eval(const Geographic_multilinestring *g1,
            const Geographic_linestring *g2) const;
  bool eval(const Geographic_multilinestring *g1,
            const Geographic_polygon *g2) const;
  bool eval(const Geographic_multilinestring *g1,
            const Geographic_geometrycollection *g2) const;
  bool eval(const Geographic_multilinestring *g1,
            const Geographic_multipoint *g2) const;
  bool eval(const Geographic_multilinestring *g1,
            const Geographic_multilinestring *g2) const;
  bool eval(const Geographic_multilinestring *g1,
            const Geographic_multipolygon *g2) const;

  //////////////////////////////////////////////////////////////////////////////

  // intersects(Geographic_multipolygon, *)

  bool eval(const Geographic_multipolygon *g1,
            const Geographic_point *g2) const;
  bool eval(const Geographic_multipolygon *g1,
            const Geographic_linestring *g2) const;
  bool eval(const Geographic_multipolygon *g1,
            const Geographic_polygon *g2) const;
  bool eval(const Geographic_multipolygon *g1,
            const Geographic_geometrycollection *g2) const;
  bool eval(const Geographic_multipolygon *g1,
            const Geographic_multipoint *g2) const;
  bool eval(const Geographic_multipolygon *g1,
            const Geographic_multilinestring *g2) const;
  bool eval(const Geographic_multipolygon *g1,
            const Geographic_multipolygon *g2) const;

  //////////////////////////////////////////////////////////////////////////////

  // intersects(Box, Box)

  bool eval(const Cartesian_box *b1, const Cartesian_box *b2) const;
  bool eval(const Geographic_box *b1, const Geographic_box *b2) const;
};

}  // namespace gis

#endif  // SQL_GIS_INTERSECTS_FUNCTOR_H_INCLUDED

// This file is part of the dune-stuff project:
//   https://users.dune-project.org/projects/dune-stuff/
// Copyright holders: Rene Milk, Felix Schindler
// License: BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)

#ifndef DUNE_STUFF_GRID_FAKEENTITY_HH
#define DUNE_STUFF_GRID_FAKEENTITY_HH

#include <dune/stuff/common/disable_warnings.hh>
# include <dune/geometry/type.hh>
#include <dune/stuff/common/reenable_warnings.hh>

namespace Dune {
namespace Stuff {
namespace Grid {


template< int dd >
class FakeEntity
{
public:
  GeometryType type() const;

  class Geometry
  {
  public:
    template< class T >
    T global(const T& t) const
    {
      return t;
    }
  }; // class Geometry

  Geometry geometry() const
  {
    return Geometry();
  }

  static constexpr int dimension = dd;
}; // class FakeEntity


} // namespace Grid
} // namespace Stuff
} // namespace Dune

#endif // DUNE_STUFF_GRID_FAKEENTITY_HH

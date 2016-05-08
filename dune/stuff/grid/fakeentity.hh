// This file is part of the dune-stuff project:
//   https://github.com/wwu-numerik/dune-stuff
// The copyright lies with the authors of this file (see below).
// License: BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)
// Authors:
//   Felix Schindler (2012 - 2015)
//   Rene Milk       (2011 - 2012, 2014 - 2015)

#ifndef DUNE_STUFF_GRID_FAKEENTITY_HH
#define DUNE_STUFF_GRID_FAKEENTITY_HH

#include <dune/geometry/type.hh>

namespace Dune {
namespace Stuff {
namespace Grid {

template <int dd>
class FakeEntity
{
public:
  GeometryType type() const;

  class Geometry
  {
  public:
    template <class T>
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

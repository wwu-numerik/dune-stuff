// This file is part of the dune-stuff project:
//   http://users.dune-project.org/projects/dune-stuff/
// Copyright Holders: Felix Albrecht, Rene Milk
// License: BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)

#ifndef DUNE_STUFF_GRID_FAKEENTITY_HH
#define DUNE_STUFF_GRID_FAKEENTITY_HH

#include <dune/geometry/type.hh>

namespace Dune {
namespace Stuff {
namespace Grid {


template< int dd >
class FakeEntity
{
public:
  GeometryType type() const;
}; // class FakeEntity


} // namespace Grid
} // namespace Stuff
} // namespace Dune

#endif // DUNE_STUFF_GRID_FAKEENTITY_HH

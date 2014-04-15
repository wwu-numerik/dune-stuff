// This file is part of the dune-stuff project:
//   http://users.dune-project.org/projects/dune-stuff/
// Copyright Holders: Rene Milk, Felix Schindler
// License: BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)

#ifdef HAVE_CMAKE_CONFIG
# include "cmake_config.h"
#elif defined (HAVE_CONFIG_H)
# include "config.h"
#endif

#include "fakeentity.hh"

namespace Dune {
namespace Stuff {
namespace Grid {


template< int dd >
GeometryType FakeEntity< dd >::type() const
{
  return GeometryType(GeometryType::simplex, dd);
}


} // namespace Grid
} // namespace Stuff
} // namespace Dune

template class Dune::Stuff::Grid::FakeEntity< 1 >;
template class Dune::Stuff::Grid::FakeEntity< 2 >;
template class Dune::Stuff::Grid::FakeEntity< 3 >;

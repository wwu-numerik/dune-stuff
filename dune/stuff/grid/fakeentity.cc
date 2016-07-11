// This file is part of the dune-stuff project:
//   https://github.com/wwu-numerik/dune-stuff
// The copyright lies with the authors of this file (see below).
// License: BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)
// Authors:
//   Felix Schindler (2013 - 2015)
//   Rene Milk       (2015)

#include "config.h"

#include "fakeentity.hh"

namespace Dune {
namespace Stuff {
namespace Grid {

template <int dd>
GeometryType FakeEntity<dd>::type() const
{
  return GeometryType(GeometryType::simplex, dd);
}

} // namespace Grid
} // namespace Stuff
} // namespace Dune

template class Dune::Stuff::Grid::FakeEntity<1>;
template class Dune::Stuff::Grid::FakeEntity<2>;
template class Dune::Stuff::Grid::FakeEntity<3>;

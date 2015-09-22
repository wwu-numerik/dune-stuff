// This file is part of the dune-stuff project:
//   https://github.com/wwu-numerik/dune-stuff
// Copyright holders: Rene Milk, Felix Schindler
// License: BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)

#include "main.hxx"

#include <dune/stuff/grid/provider/cube.hh>

#include "grid_provider.hh"

#if HAVE_DUNE_GRID

template< class GridType >
struct CubeGridProvider
  : public GridProviderBase< Dune::Stuff::Grid::Providers::Cube< GridType > >
{};


typedef testing::Types< SGRIDS
                      , YASPGRIDS
# if HAVE_ALUGRID
                      , ALUGRIDS
# endif
                      > GridTypes;

TYPED_TEST_CASE(CubeGridProvider, GridTypes);
TYPED_TEST(CubeGridProvider, is_default_creatable)
{
  this->is_default_creatable();
}
TYPED_TEST(CubeGridProvider, fulfills_const_interface)
{
  this->const_interface();
}
TYPED_TEST(CubeGridProvider, is_visualizable)
{
  this->visualize();
}
TYPED_TEST(CubeGridProvider, fulfills_non_const_interface)
{
  this->non_const_interface();
}


#else // HAVE_DUNE_GRID

TEST(DISABLED_CubeGridProvider, is_default_creatable) {}
TEST(DISABLED_CubeGridProvider, fulfills_const_interface) {}
TEST(DISABLED_CubeGridProvider, is_visualizable) {}

#endif // HAVE_DUNE_GRID


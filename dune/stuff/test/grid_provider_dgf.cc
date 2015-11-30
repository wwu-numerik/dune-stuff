// This file is part of the dune-stuff project:
//   https://github.com/wwu-numerik/dune-stuff
// Copyright holders: Rene Milk, Felix Schindler
// License: BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)

#define DUNE_STUFF_TEST_MAIN_CATCH_EXCEPTIONS 1

#include "main.hxx"

#include <dune/stuff/grid/provider/dgf.hh>

#include "grid_provider.hh"

#if HAVE_DUNE_GRID

template < class GridType >
struct DgfGridProvider : public GridProviderBase< Dune::Stuff::Grid::Providers::DGF< GridType > >
{
};


typedef testing::Types< SGRIDS, YASPGRIDS
#if HAVE_ALUGRID
                        ,
                        ALUGRIDS
#endif
                        > GridTypes;

TYPED_TEST_CASE(DgfGridProvider, GridTypes);
TYPED_TEST(DgfGridProvider, is_default_creatable)
{
  this->is_default_creatable();
}
TYPED_TEST(DgfGridProvider, fulfills_const_interface)
{
  this->const_interface();
}
TYPED_TEST(DgfGridProvider, is_visualizable)
{
  this->visualize();
}
TYPED_TEST(DgfGridProvider, fulfills_non_const_interface)
{
  this->non_const_interface();
}


#else // HAVE_DUNE_GRID

TEST(DISABLED_DgfGridProvider, is_default_creatable)
{
}
TEST(DISABLED_DgfGridProvider, fulfills_const_interface)
{
}
TEST(DISABLED_DgfGridProvider, is_visualizable)
{
}

#endif // HAVE_DUNE_GRID

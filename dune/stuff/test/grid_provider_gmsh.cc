// This file is part of the dune-stuff project:
//   https://github.com/wwu-numerik/dune-stuff
// The copyright lies with the authors of this file (see below).
// License: BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)
// Authors:
//   Felix Schindler (2015)

#include "main.hxx"

#include <dune/stuff/grid/provider/gmsh.hh>

#include "grid_provider.hh"

#undef HAVE_DUNE_FEM
#if HAVE_DUNE_GRID && HAVE_ALUGRID

template< class GridType >
struct GmshGridProvider
  : public GridProviderBase< Dune::Stuff::Grid::Providers::Gmsh< GridType > >
{};


// Other grid types only make sense if we have a suitable .msh file for the corresponding dimension and element type.
// If you have, do not forget to name it properly (compare the existing ones), adapt the `default_config()` method of
// `Grid::Providers::Gmsh`, and to add the filename to the linked files in `dune/stuff/test/CmakeLists.txt`.
typedef testing::Types< Dune::ALUGrid< 2, 2, Dune::simplex, Dune::conforming >
                      , Dune::ALUGrid< 2, 2, Dune::simplex, Dune::nonconforming > > GridTypes;

TYPED_TEST_CASE(GmshGridProvider, GridTypes);
TYPED_TEST(GmshGridProvider, is_default_creatable)
{
  this->is_default_creatable();
}
TYPED_TEST(GmshGridProvider, fulfills_const_interface)
{
  this->const_interface();
}
TYPED_TEST(GmshGridProvider, is_visualizable)
{
  this->visualize();
}
TYPED_TEST(GmshGridProvider, fulfills_non_const_interface)
{
  this->non_const_interface();
}

#else // HAVE_DUNE_GRID && HAVE_ALUGRID

TEST(DISABLED_GmshGridProvider, is_default_creatable) {}
TEST(DISABLED_GmshGridProvider, fulfills_const_interface) {}
TEST(DISABLED_GmshGridProvider, is_visualizable) {}

#endif // HAVE_DUNE_GRID && HAVE_ALUGRID


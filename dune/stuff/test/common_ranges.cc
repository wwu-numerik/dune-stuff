// This file is part of the dune-stuff project:
//   https://github.com/wwu-numerik/dune-stuff
// Copyright holders: Rene Milk, Felix Schindler
// License: BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)

#include "main.hxx"

#if HAVE_DUNE_GRID

#include <dune/grid/yaspgrid.hh>

#include <dune/stuff/grid/provider/cube.hh>
#include <dune/stuff/common/ranges.hh>

using namespace Dune;
using namespace Dune::Stuff;
using namespace Dune::Stuff::Common;
using namespace Dune::Stuff::Grid;
using namespace std;

//! \TODO enable embedded grids
typedef testing::Types<YaspGrid<1, EquidistantOffsetCoordinates<double, 1>>,
                       YaspGrid<2, EquidistantOffsetCoordinates<double, 2>>,
                       YaspGrid<3, EquidistantOffsetCoordinates<double, 3>>,
                       YaspGrid<4, EquidistantOffsetCoordinates<double, 4>>> Grids;

template <class T>
struct CornerRangeTest : public ::testing::Test
{
  static const size_t level = 4;
  typedef T GridType;
  static const size_t dim_grid = GridType::dimension;
  const DSG::Providers::Cube<GridType> grid_prv;
  CornerRangeTest() : grid_prv(0., 1., level) {}

  template <typename Entity, typename RangeType>
  void check_range(const Entity& e, RangeType range)
  {
    auto i  = e.geometry().corners();
    auto ci = std::pow(2, dim_grid);
    EXPECT_EQ(i, ci);
    for (auto corner : range) {
      for (auto c : corner) {
        EXPECT_FALSE(DSC::FloatCmp::lt(c, decltype(c)(0)));
        EXPECT_FALSE(DSC::FloatCmp::gt(c, decltype(c)(1)));
      }
      i--;
    }
    EXPECT_EQ(i, 0);
  }

  void check()
  {
    const auto gv = grid_prv.grid().leafGridView();
    const auto DUNE_UNUSED(entities) = gv.size(0);
    const auto end = gv.template end<0>();
    for (auto it = gv.template begin<0>(); it != end; ++it) {
      check_range(*it, cornerRange(it->geometry()));
      check_range(*it, cornerRange(*it));
    }
    for (auto v : DSC::valueRange(T::dimensionworld)) {
      EXPECT_GE(v, 0);
    }
  }
};

TYPED_TEST_CASE(CornerRangeTest, Grids);
TYPED_TEST(CornerRangeTest, Misc) { this->check(); }

#endif // #if HAVE_DUNE_GRID

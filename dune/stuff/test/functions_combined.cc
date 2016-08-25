// This file is part of the dune-stuff project:
//   https://github.com/wwu-numerik/dune-stuff
// Copyright holders: Rene Milk, Felix Schindler
// License: BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)

#include "main.hxx"

#include <memory>

#include <boost/numeric/conversion/cast.hpp>

#if HAVE_DUNE_GRID
#include <dune/grid/sgrid.hh>
#endif

#include <dune/geometry/quadraturerules.hh>

#include <dune/stuff/common/float_cmp.hh>
#include <dune/stuff/grid/provider/cube.hh>
#include <dune/stuff/functions/constant.hh>
#include <dune/stuff/functions/combined.hh>

#include "functions.hh"

#if HAVE_DUNE_GRID

using namespace Dune;
using namespace Stuff;


template <class G>
class DifferenceFunctionType
{
  typedef typename G::template Codim<0>::Entity E;
  typedef typename G::ctype D;
  static const size_t d = G::dimension;
  typedef double R;
  static const size_t r = 1;
  static const size_t rC = 1;

public:
  typedef Functions::Constant<E, D, d, R, r, rC> ConstantFunctionType;
  typedef Functions::Difference<ConstantFunctionType, ConstantFunctionType> value;
}; // struct DifferenceFunctionType


template <class DimDomain>
class DifferenceFunctionTest
    : public FunctionTest<typename DifferenceFunctionType<SGrid<DimDomain::value, DimDomain::value>>::value>
{
protected:
  typedef SGrid<DimDomain::value, DimDomain::value> GridType;
  typedef typename DifferenceFunctionType<GridType>::value FunctionType;

  static std::shared_ptr<GridType> create_grid()
  {
    return Stuff::Grid::Providers::Cube<GridType>(0.0, 1.0, 4).grid_ptr();
  }

  static std::unique_ptr<FunctionType> create(const double ll, const double rr)
  {
    typedef typename DifferenceFunctionType<SGrid<DimDomain::value, DimDomain::value>>::ConstantFunctionType
        ConstantFunctionType;
    auto left = std::make_shared<ConstantFunctionType>(ll);
    auto right = std::make_shared<ConstantFunctionType>(rr);
    return std::unique_ptr<FunctionType>(new FunctionType(left, right));
  } // ... create(...)
}; // class DifferenceFunctionTest


typedef testing::Types<Int<1>, Int<2>, Int<3>> DimDomains;

TYPED_TEST_CASE(DifferenceFunctionTest, DimDomains);
TYPED_TEST(DifferenceFunctionTest, static_interface_check)
{
  this->static_interface_check();
}
TYPED_TEST(DifferenceFunctionTest, dynamic_interface_check)
{
  this->dynamic_interface_check(*(this->create(1.0, 1.0)), *(this->create_grid()));
}
TYPED_TEST(DifferenceFunctionTest, evaluate_check)
{
  auto grid_ptr = this->create_grid();
  auto func = this->create(1.0, 2.0);
  //  func->visualize(grid_ptr->leafGridView(), "foo");
  for (const auto& entity : Stuff::Common::entityRange(grid_ptr->leafGridView())) {
    const auto local_func = func->local_function(entity);
    const auto& quadrature = QuadratureRules<double, TypeParam::value>::rule(
        entity.type(), boost::numeric_cast<int>(local_func->order() + 2));
    for (const auto& element : quadrature) {
      const auto& local_point = element.position();
      const auto val = local_func->evaluate(local_point);
      EXPECT_EQ(val[0], -1.0);
    }
  }
} // DifferenceFunctionTest, evaluate_check


#else // HAVE_DUNE_GRID

// no-compile placeholders to mark disabled tests in test-binary output
TEST(DISABLED_FlatTopFunctionTest, static_interface_check)
{
}
TEST(DISABLED_DifferenceFunctionTest, dynamic_interface_check)
{
}
TEST(DISABLED_DifferenceFunctionTest, evaluate_check)
{
}

#endif // HAVE_DUNE_GRID

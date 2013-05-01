#include "test_common.hh"

#include <memory>

#include <dune/common/exceptions.hh>
#include <dune/common/typetraits.hh>
#include <dune/common/static_assert.hh>

#include <dune/grid/sgrid.hh>

#include <dune/stuff/function/constant.hh>
#include <dune/stuff/grid/provider.hh>

using namespace Dune::Stuff;


typedef testing::Types< FunctionConstant< double, 1, double, 1 >
                      , FunctionConstant< double, 2, double, 1 >
                      , FunctionConstant< double, 3, double, 1 >
                      > LocalizableFunctions;

template< class FunctionType >
struct LocalizableFunctionTest
  : public ::testing::Test
{
  static_assert((Dune::IsBaseOf< LocalizableFunction, FunctionType >::value), "ERROR: not a LocalizableFunction!");

  typedef typename FunctionType::DomainFieldType DomainFieldType;
  static const unsigned int dimDomain = FunctionType::dimDomain;
  typedef typename FunctionType::DomainType DomainType;
  typedef typename FunctionType::RangeFieldType RangeFieldType;
  static const unsigned int dimRangeRows = FunctionType::dimRangeRows;
  static const unsigned int dimRangeCols = FunctionType::dimRangeCols;
  typedef typename FunctionType::RangeType RangeType;
  typedef Dune::SGrid< dimDomain, dimDomain > GridType;
  typedef GridProviderCube< GridType > GridProviderType;
  typedef typename GridType::LeafGridView GridViewType;
//  typedef typename GridType::template Codim< 0 >::Entity EntityType;
//  typedef typename FunctionType::template LocalizedFunction< EntityType, DomainFieldType, dimDomain, RangeFieldType, dimRangeRows, dimRangeCols >::Type LocalFunctionType;

  void check() const
  {
    // generate grid
    const GridProviderType* gridProvider = GridProviderType::create(GridProviderType::createSampleDescription());
    const std::shared_ptr< const GridType > grid = gridProvider->grid();
    const GridViewType gridView = grid->leafView();
    const auto entityIt = gridView.template begin< 0 >();
    const auto& entity = *entityIt;

    // create function
    const FunctionType* function = FunctionType::create(FunctionType::createSampleDescription());

    // get localfunction
    const auto localFunction =function->localFunction(entity);
    const int order = localFunction.order();
    const auto& ent = localFunction.entity();
    const DomainType x(1);
    RangeType ret(0);
    localFunction.evaluate(x, ret);

    // clean up
    delete function;
    delete gridProvider;
  }
}; // struct LocalizableFunctionTest


TYPED_TEST_CASE(LocalizableFunctionTest, LocalizableFunctions);
TYPED_TEST(LocalizableFunctionTest, LocalizableFunction) {
  this->check();
}


int main(int argc, char** argv)
{
  test_init(argc, argv);
  return RUN_ALL_TESTS();
}

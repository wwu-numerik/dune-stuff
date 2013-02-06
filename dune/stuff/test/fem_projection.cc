#include "test_common.hh"

#if HAVE_DUNE_FEM

#include <dune/fem/misc/mpimanager.hh>
#include <dune/fem/space/fvspace/fvspace.hh>
#include <dune/fem/space/dgspace.hh>
#include <dune/fem/gridpart/adaptiveleafgridpart.hh>
#include <dune/fem/function/adaptivefunction.hh>
#include <dune/fem/operator/projection/l2projection.hh>
#include <dune/fem/io/file/datawriter.hh>

#include <dune/stuff/fem/customprojection.hh>
#include <dune/stuff/grid/provider.hh>
#include <dune/stuff/function/expression.hh>
#include <dune/stuff/function/parametric/separable/coefficient.hh>

TEST(Projection, All) {
  typedef Dune::Stuff::Grid::Provider::Interface<> GridProviderType;
  GridProviderType* gridProvider = Dune::Stuff::Grid::Provider::create<>();
  typedef typename GridProviderType::GridType GridType;
  typedef Dune::AdaptiveLeafGridPart< GridType > GridPartType;
  GridType& grid = *(gridProvider->grid());
  GridPartType gridPart(grid);
  typedef Dune::Stuff::Function::Expression< double, 2, double, 1 > ScalarFunctionType;
  typedef ScalarFunctionType::FunctionSpaceType FSpaceType;
  typedef Dune::DiscontinuousGalerkinSpace< FSpaceType,
                                            GridPartType,
                                            1 >
    DiscreteFunctionSpaceType;
  typedef Dune::AdaptiveDiscreteFunction< DiscreteFunctionSpaceType > DiscreteFunctionType;
  DiscreteFunctionSpaceType disc_space(gridPart);
  DiscreteFunctionType rf_disc("rf", disc_space);
  typedef Dune::tuple< const DiscreteFunctionType* > OutputTupleType;

  Dune::Stuff::Fem::BetterL2Projection::project(scalar_f_from_single_expression, rf_disc);

}
#endif

int main(int argc, char** argv)
{
  testing::InitGoogleTest(&argc, argv);
  Dune::MPIHelper::instance(argc, argv);
  return RUN_ALL_TESTS();
}

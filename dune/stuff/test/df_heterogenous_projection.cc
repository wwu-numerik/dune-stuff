#include "test_common.hh"

#if HAVE_DUNE_GRID

#include <iostream>
#include <fstream>
#include <utility>
#include <boost/filesystem.hpp>

#include <dune/common/mpihelper.hh>
#include <dune/common/parametertreeparser.hh>

#include <dune/stuff/grid/provider/cube.hh>
#include <dune/stuff/discretefunction/projection/heterogenous.hh>

using namespace Dune;

static const int dim = 2;
typedef testing::Types< Dune::YaspGrid< dim >
#if HAVE_ALUGRID
  , Dune::ALUCubeGrid< dim, dim >
  , Dune::ALUConformGrid< dim, dim >
  , Dune::ALUSimplexGrid< dim, dim >
#endif
#if HAVE_ALBERTA
  , Dune::AlbertaGrid< dim >
#endif
#if HAVE_UG
  , Dune::UGGrid< dim >
#endif
  , Dune::SGrid< dim, dim > > GridTypes;

#if HAVE_DUNE_FEM

#include <dune/fem/gridpart/adaptiveleafgridpart.hh>
#include <dune/fem/function/adaptivefunction/adaptivefunction.hh>
#include <dune/fem/space/common/functionspace.hh>
#include <dune/fem/space/lagrangespace.hh>

//typedef Dune::ALUSimplexGrid< dim, dim > T1;
typedef Dune::YaspGrid<dim> SourceGrid;
typedef SourceGrid TargetGrid;

template <class Grid>
struct Traits {
  typedef AdaptiveLeafGridPart<Grid> GridPart;
  typedef FunctionSpace<double, double, dim,1> ContFunctionSpace;
  typedef LagrangeDiscreteFunctionSpace<ContFunctionSpace, AdaptiveLeafGridPart<Grid>, 1> DiscreteSpace;
  typedef Fem::AdaptiveDiscreteFunction<DiscreteSpace> DiscreteFunction;
};

TEST(Projection, Cubes){
  const int macro_elements = 4;
  typedef Traits<SourceGrid> SourceTraits;
  typedef Traits<TargetGrid> TargetTraits;
  auto source_cube = Stuff::Grid::Provider::UnitCube<SourceGrid>(macro_elements).gridPtr();
  auto target_cube = Stuff::Grid::Provider::UnitCube<TargetGrid>(macro_elements * 3).gridPtr();
  typename SourceTraits::GridPart source_part(*source_cube);
  typename TargetTraits::GridPart target_part(*target_cube);

  typename SourceTraits::DiscreteSpace source_space(source_part);
  typename TargetTraits::DiscreteSpace target_space(target_part);

  typename SourceTraits::DiscreteFunction source_df("source", source_space);
  typename TargetTraits::DiscreteFunction target_df("target", target_space);

//  Stuff::HeterogenousProjection::project(source_df, target_df);
}

#endif //#if HAVE_DUNE_FEM

#endif // #if HAVE_DUNE_GRID

int main(int argc, char** argv)
{
  testing::InitGoogleTest(&argc, argv);
  Dune::MPIHelper::instance(argc, argv);
  return RUN_ALL_TESTS();
}

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
#include <dune/stuff/function/expression.hh>
#include <dune/stuff/fem/customprojection.hh>

using namespace Dune;

static const int dim = 2;
typedef testing::Types< Dune::YaspGrid< dim >
#if HAVE_ALUGRID
  , Dune::ALUCubeGrid< dim, dim >
  , Dune::ALUConformGrid< dim, dim >
  , Dune::ALUSimplexGrid< dim, dim >
#endif
#if HAVE_ALBERTA
  , Dune::AlbertaGrid< dim, dim >
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
#include <dune/fem/io/file/vtkio.hh>
#include <dune/fem/operator/lagrangeinterpolation.hh>

#if HAVE_ALUGRID
typedef Dune::ALUConformGrid< dim, dim > SourceGrid;
#else
typedef Dune::SGrid<dim,dim> SourceGrid;
#endif
typedef Dune::YaspGrid<dim> TargetGrid;

template <class Grid>
struct Traits {
  typedef AdaptiveLeafGridPart<Grid> GridPart;
  typedef FunctionSpace<double, double, dim,1> ContFunctionSpace;
  typedef LagrangeDiscreteFunctionSpace<ContFunctionSpace, AdaptiveLeafGridPart<Grid>, 1> DiscreteSpace;
  typedef Fem::AdaptiveDiscreteFunction<DiscreteSpace> DiscreteFunction;
};

template<class GridPart, class DF>
void vtk_out(const GridPart& part, const DF& df) {
  Dune::VTKIO<GridPart> vtkWriter(part);
  const std::string vtkWriterFilename = std::string("df_hetereogenous_projection_data_") + df.name();
  vtkWriter.addVertexData(df);
  vtkWriter.write( vtkWriterFilename.c_str() );
}

void ptest() {
  const int macro_elements = 4;
  typedef Traits<SourceGrid> SourceTraits;
  typedef Traits<TargetGrid> TargetTraits;
  auto source_cube = Stuff::Grid::Provider::Cube<SourceGrid>(0,1,macro_elements).grid();
  auto target_cube = Stuff::Grid::Provider::Cube<TargetGrid>(0,1,macro_elements * 3).grid();
  source_cube->globalRefine(2);
  target_cube->globalRefine(3);
  typename SourceTraits::GridPart source_part(*source_cube);
  typename TargetTraits::GridPart target_part(*target_cube);

  typename SourceTraits::DiscreteSpace source_space(source_part);
  typename TargetTraits::DiscreteSpace target_space(target_part);

  typename SourceTraits::DiscreteFunction source_df("source", source_space);
  typename TargetTraits::DiscreteFunction target_df("target", target_space);


  typedef Dune::Stuff::Function::Expression< SourceGrid::ctype, SourceGrid::dimension, SourceGrid::ctype, 1 > ScalarFunctionType;
  ScalarFunctionType scalar_f("x", "x[0] + x[1]");
  Dune::LagrangeInterpolation<typename SourceTraits::DiscreteFunction>::apply(scalar_f, source_df);
//  Dune::Stuff::Fem::BetterL2Projection::project(scalar_f, source_df);

  Stuff::HeterogenousProjection<>::project(source_df, target_df);

  vtk_out(source_part, source_df);
  vtk_out(target_part, target_df);
}

TEST(Projection, Cubes){
  ptest();
}

#endif //#if HAVE_DUNE_FEM

#endif // #if HAVE_DUNE_GRID

int main(int argc, char** argv)
{
//  testing::InitGoogleTest(&argc, argv);
  Dune::MPIManager::initialize(argc,argv);

//  return RUN_ALL_TESTS();
  ptest();
}

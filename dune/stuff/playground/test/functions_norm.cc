// This file is part of the dune-stuff project:
//   https://github.com/wwu-numerik/dune-stuff
// Copyright holders: Rene Milk, Felix Schindler
// License: BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)

#include "test_common.hh"

#include <unordered_map>
#include <dune/common/mpihelper.hh>
#include <dune/common/parametertreeparser.hh>

#include <dune/stuff/grid/provider/cube.hh>

#include <dune/stuff/functions/constant.hh>
#include <dune/stuff/functions/expression.hh>
#include <dune/stuff/common/profiler.hh>
#include <dune/stuff/common/ranges.hh>
#include <dune/stuff/playground/functions/femadapter.hh>
#include <dune/stuff/playground/functions/norm.hh>
#include <dune/stuff/playground/functions/pdelabadapter.hh>
#include <dune/stuff/aliases.hh>

#if HAVE_DUNE_FEM
# include <dune/fem/gridpart/adaptiveleafgridpart.hh>
# include <dune/fem/function/adaptivefunction/adaptivefunction.hh>
# include <dune/fem/space/common/functionspace.hh>
# include <dune/fem/space/lagrange.hh>
# include <dune/fem/io/file/vtkio.hh>
# include <dune/fem/operator/lagrangeinterpolation.hh>
#endif // HAVE_DUNE_FEM

#if HAVE_DUNE_PDELAB
# include <dune/pdelab/common/function.hh>
# include <dune/pdelab/gridfunctionspace/gridfunctionspace.hh>
# include <dune/pdelab/backend/istl/descriptors.hh>
# include <dune/pdelab/backend/istl/bcrsmatrixbackend.hh>
# include <dune/pdelab/finiteelementmap/qkfem.hh>
# include <dune/pdelab/backend/istlvectorbackend.hh>
# include <dune/pdelab/backend/backendselector.hh>
# include <dune/pdelab/constraints/conforming.hh>
# include <dune/pdelab/gridfunctionspace/interpolate.hh>
#endif
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

typedef YaspGrid<dim> SourceGrid;
typedef std::unordered_map<std::string, double> ResultMap;

template <class Grid>
struct GTraits {
  typedef double FieldType;
  static constexpr int polynomial_order = 1;
  typedef DSG::Providers::Cube<Grid> GridProvider;
  typedef typename Grid::template Codim<0>::Entity Entity;
  typedef Stuff::Functions::Constant<Entity, typename Grid::ctype,
      Grid::dimension, typename Grid::ctype, 1> ConstantFunction;
  typedef Stuff::Functions::Expression< Entity, typename Grid::ctype,
      Grid::dimension, typename Grid::ctype, 1 > ScalarFunction;
#if HAVE_DUNE_FEM
  typedef Dune::Fem::AdaptiveLeafGridPart<Grid> GridPart;
  typedef Dune::Fem::FunctionSpace<FieldType, FieldType, dim,1> ContFunctionSpace;
  typedef Dune::Fem::LagrangeDiscreteFunctionSpace<ContFunctionSpace, Dune::Fem::AdaptiveLeafGridPart<Grid>, polynomial_order> DiscreteSpace;
  typedef Fem::AdaptiveDiscreteFunction<DiscreteSpace> DiscreteFunction;
#endif
#if HAVE_DUNE_PDELAB
  typedef PDELab::QkLocalFiniteElementMap<typename Grid::LeafGridView, typename Grid::ctype,FieldType,polynomial_order>
  FEMap;
  typedef Dune::PDELab::ISTLVectorBackend<> VectorBackend;
  typedef Dune::PDELab::istl::BCRSMatrixBackend<> MatrixBackend;
  typedef PDELab::GridFunctionSpace<typename Grid::LeafGridView,FEMap,PDELab::ConformingDirichletConstraints,
            VectorBackend> GridFunctionSpace;
  typedef typename PDELab::BackendVectorSelector<GridFunctionSpace,FieldType>::Type PdelabVector;

#endif
};

template <class Grid>
std::shared_ptr<Grid> make_grid(const int macro_elements) {
  auto source_cube = typename GTraits<Grid>::GridProvider(0,1,macro_elements).grid();
  source_cube->globalRefine(2);
  return source_cube;
}

void output(const ResultMap& values) {
  for(auto pair : values) {
    DSC_LOG_INFO << (boost::format("%s\t%e\n") % pair.first % pair.second).str();
  }
}

template <class Grid>
void fem(const int macro_elements, ResultMap& values)
{
  typedef GTraits<Grid> Traits;

  auto grid = make_grid<Grid>(macro_elements);
  typename Traits::GridPart part(*grid);
  typename Traits::DiscreteSpace space(part);


  typename Traits::ScalarFunction linear_gradient("x", "x[0]");
  typename Traits::DiscreteFunction linear_gradient_df("", space);
  typename Traits::ConstantFunction constant_0(0);
  typename Traits::ConstantFunction constant_neg1(-1);
  typename Traits::DiscreteFunction constant_0_df("", space);
  typename Traits::DiscreteFunction constant_neg1_df("", space);

  Fem::LagrangeInterpolation<typename Traits::ScalarFunction,typename Traits::DiscreteFunction>
      ::apply(linear_gradient, linear_gradient_df);
  Fem::LagrangeInterpolation<typename Traits::ConstantFunction,typename Traits::DiscreteFunction>
      ::apply(constant_0, constant_0_df);
  Fem::LagrangeInterpolation<typename Traits::ConstantFunction,typename Traits::DiscreteFunction>
      ::apply(constant_neg1, constant_neg1_df);

  values["constant_0_l2"] = DS::l2norm(constant_0_df);
  values["constant_0_h1"] = DS::h1norm(constant_0_df);
  values["constant_neg1_l2"] = DS::l2norm(constant_neg1_df);
  values["constant_neg1_h1"] = DS::h1norm(constant_neg1_df);
}

template <class Grid>
void pdelab(const int macro_elements, ResultMap& values)
{
  typedef GTraits<Grid> Traits;

  auto grid = make_grid<Grid>(macro_elements);
  const auto& grid_view = grid->leafGridView();
  typename Traits::FEMap fe_map(grid_view);
  typename Traits::ScalarFunction linear_gradient_ltr("x", "x[0]", 1, "id", {{"1", "0"}});
  typename Traits::ScalarFunction linear_gradient_btt("x", "x[1]", 1, "id", {{"0", "1"}});
  typename Traits::GridFunctionSpace space(grid_view, fe_map);
  typename Traits::ConstantFunction constant_0(0);
  typename Traits::ConstantFunction constant_neg1(-1);
  typename Traits::PdelabVector constant_0_pde(space, 0.0);
  typename Traits::PdelabVector constant_neg1_pde(space, -1.0);
  typename Traits::PdelabVector linear_gradient_ltr_pde(space, 0.0);
  typename Traits::PdelabVector linear_gradient_btt_pde(space, 0.0);
  Dune::PDELab::interpolate(DS::pdelabAdapted(linear_gradient_ltr, grid_view), space, linear_gradient_ltr_pde);
  Dune::PDELab::interpolate(DS::pdelabAdapted(linear_gradient_btt, grid_view), space, linear_gradient_btt_pde);

  values["constant_0_l2_diff"] = DS::l2distance(constant_0, constant_0_pde);
  values["constant_0_h1_diff"] = DS::h1distance(constant_0, constant_0_pde);
  values["constant_neg1_l2_diff"] = DS::l2distance(constant_neg1, constant_neg1_pde);
  values["constant_neg1_h1_diff"] = DS::h1distance(constant_neg1, constant_neg1_pde);
  values["scalar_grad_ltr_l2_diff"] = DS::l2distance(linear_gradient_ltr, linear_gradient_ltr_pde);
  values["scalar_grad_ltr_h1_diff"] = DS::h1distance(linear_gradient_ltr, linear_gradient_ltr_pde);
  values["scalar_grad_ltr_to_0_l2_diff"] = DS::l2distance(constant_0, linear_gradient_ltr_pde);
  values["scalar_grad_ltr_to_0_h1_diff"] = DS::h1distance(constant_0, linear_gradient_ltr_pde);
  values["scalar_grad_btt_l2_diff"] = DS::l2distance(linear_gradient_btt, linear_gradient_btt_pde);
  values["scalar_grad_btt_h1_diff"] = DS::h1distance(linear_gradient_btt, linear_gradient_btt_pde);
  values["scalar_grad_btt_to_0_l2_diff"] = DS::l2distance(constant_0, linear_gradient_btt_pde);
  values["scalar_grad_btt_to_0_h1_diff"] = DS::h1distance(constant_0, linear_gradient_btt_pde);
}

TEST(Norm, All){
  const int runs = DSC_CONFIG_GET("runs", 1);
  DSC_PROFILER.reset(runs);
  for (auto i : DSC::valueRange(runs)) {
    const auto el = std::pow(8, i+1);
    ResultMap values;
#if HAVE_DUNE_FEM
    fem<SourceGrid>(el, values);
    DSC_LOG_INFO << "dune-fem results:\n";
    output(values);
#endif // HAVE_DUNE_FEM
#if HAVE_DUNE_PDELAB
    pdelab<SourceGrid>(el, values);
    DSC_LOG_INFO << "dune-pdelab results:\n";
    output(values);
#endif // HAVE_DUNE_PDELAB
    DSC_PROFILER.nextRun();
  }
  DSC_PROFILER.outputTimingsAll();
  DSC_LOG_INFO << std::endl;
}

int main(int argc, char** argv)
{
  test_init(argc, argv);
  return RUN_ALL_TESTS();
}


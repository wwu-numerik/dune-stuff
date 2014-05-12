// This file is part of the dune-stuff project:
//   https://users.dune-project.org/projects/dune-stuff
// Copyright holders: Rene Milk, Felix Schindler
// License: BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)

#include "test_common.hh"

#if HAVE_DUNE_GRID

#include <iostream>
#include <fstream>
#include <utility>
#include <boost/filesystem.hpp>
#include <boost/format.hpp>

#include <dune/common/mpihelper.hh>
#include <dune/common/parametertreeparser.hh>

#include <dune/stuff/grid/provider/cube.hh>
#include <dune/stuff/discretefunction/projection/heterogenous.hh>
#include <dune/stuff/functions/expression.hh>
#include <dune/stuff/fem/customprojection.hh>
#include <dune/stuff/common/profiler.hh>
#include <dune/stuff/common/ranges.hh>
#include <dune/stuff/functions/femadapter.hh>
#include <dune/stuff/common/parameter/configcontainer.hh>
#include <dune/stuff/fem/namespace.hh>
#include <dune/stuff/aliases.hh>

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
#include <dune/fem/space/lagrange.hh>
#include <dune/fem/io/file/vtkio.hh>
#include <dune/fem/operator/lagrangeinterpolation.hh>

#if HAVE_ALUGRID
typedef ALUConformGrid< dim, dim > SourceGrid;
#else
typedef SGrid<dim,dim> SourceGrid;
#endif
typedef YaspGrid<dim> TargetGrid;

/**
 * throw in a LocalizableFunctionInterface derived class and use this adapter in Dune::Fem classes
 */
template< class EntityImp, class DomainFieldImp, int domainDim, class RangeFieldImp, int rangeDim >
class
      FemFunctionAdapter
    : public Dune::Fem::Function< Dune::Fem::FunctionSpace< DomainFieldImp, RangeFieldImp, domainDim, rangeDim >,
                                  DS::FunctionInterface< DomainFieldImp, domainDim, RangeFieldImp, rangeDim > >
    , public Dune::Fem::HasLocalFunction
{
  typedef DS::LocalizableFunctionInterface
      < EntityImp, DomainFieldImp, domainDim, RangeFieldImp, rangeDim > InterfaceType;
  typedef FemFunctionAdapter< EntityImp, DomainFieldImp, domainDim, RangeFieldImp, rangeDim > ThisType;

  typedef EntityImp EntityType;
  typedef std::unique_ptr<typename InterfaceType::LocalfunctionType> LocalFunctionPtrType;

  //! hide the virtual lf ptr
  class LocalFunction
  {
  public:
    LocalFunction(LocalFunctionPtrType ptr)
      : lf_ptr_(std::move(ptr))
    {}

    void evaluate(const typename InterfaceType::DomainType& xx, typename InterfaceType::RangeType& ret) const
    {
      lf_ptr_->evaluate(xx, ret);
    }

    template< class Quadrature >
    void evaluate(const Dune::Fem::QuadraturePointWrapper<Quadrature>& pw, typename InterfaceType::RangeType& ret) const
    {
      evaluate(pw.quadrature().point(pw.point()), ret);
    }


    void jacobian(const typename InterfaceType::DomainType& xx, typename InterfaceType::JacobianRangeType& ret) const
    {
      lf_ptr_->jacobian(xx, ret);
    }

  private:
    LocalFunctionPtrType lf_ptr_;
  };

public:
  typedef LocalFunction LocalFunctionType;

  FemFunctionAdapter(const InterfaceType& function)
    : function_(function)
  {}

  LocalFunction localFunction(const EntityType& entity) const {
    return LocalFunction(function_.local_function(entity));
  }

private:
  const InterfaceType& function_;
};

template < class F >
FemFunctionAdapter<typename F::EntityType, typename F::DomainFieldType, F::dimDomain, typename F::RangeFieldType, F::dimRange> femFunctionAdapter(const F& function)
{
  return FemFunctionAdapter<typename F::EntityType, typename F::DomainFieldType, F::dimDomain, typename F::RangeFieldType, F::dimRange>(function);
}

template <class Grid>
struct Traits {
  typedef Dune::Fem::AdaptiveLeafGridPart<Grid> GridPart;
  typedef Dune::Fem::FunctionSpace<double, double, dim,1> ContFunctionSpace;
  typedef Dune::Fem::LagrangeDiscreteFunctionSpace<ContFunctionSpace, Dune::Fem::AdaptiveLeafGridPart<Grid>, 1> DiscreteSpace;
  typedef Fem::AdaptiveDiscreteFunction<DiscreteSpace> DiscreteFunction;
};

template<class DF>
void vtk_out(const DF& df) {
  Dune::Fem::VTKIO<typename DF::GridPartType> vtkWriter(df.space().gridPart());
  const std::string vtkWriterFilename = std::string("df_hetereogenous_projection_data_") + df.name();
  vtkWriter.addVertexData(df);
  vtkWriter.write( vtkWriterFilename.c_str() );
}

/** we use this class to "hide" the discrete function from the lagrange interpolation
 * thereby forcing the usage of the global evaluate method we're actually interested in comparing
 * with our het. search strategies
 **/
template <class DiscreteFunctionImp>
class DiscretefunctionShroud : public Fem::Function<typename DiscreteFunctionImp::FunctionSpaceType,
                                                    DiscretefunctionShroud<DiscreteFunctionImp> >
{
  const DiscreteFunctionImp& func_;
  typedef Fem::Function<typename DiscreteFunctionImp::FunctionSpaceType,
                        DiscretefunctionShroud<DiscreteFunctionImp> >
    BaseType;
public:
  DiscretefunctionShroud(const DiscreteFunctionImp& func)
    : func_(func)
  {}

  void evaluate ( const typename BaseType::DomainType &x, typename BaseType::RangeType &ret ) const
  {
    func_.evaluate(x, ret);
  }
};


template <class SourceGrid, class TargetGrid>
void ptest(const std::shared_ptr<SourceGrid>& source_cube, const std::shared_ptr<TargetGrid>& target_cube, bool do_lagrange = false)
{
  typedef Traits<SourceGrid> SourceTraits;
  typedef Traits<TargetGrid> TargetTraits;
  std::cout << (boost::format("\n\nProjecting from %d source elements to %d target elements\n")
                   % source_cube->size(0) % target_cube->size(0)).str();
  typename SourceTraits::GridPart source_part(*source_cube);
  typename TargetTraits::GridPart target_part(*target_cube);

  typename SourceTraits::DiscreteSpace source_space(source_part);
  typename TargetTraits::DiscreteSpace target_space(target_part);

  typename SourceTraits::DiscreteFunction source_df("source", source_space);
  typename TargetTraits::DiscreteFunction target_df("target", target_space);
  typename TargetTraits::DiscreteFunction fem_target_df("fem_target", target_space);

  typedef typename SourceGrid::template Codim<0>::Entity SourceEntity;
  typedef typename TargetGrid::template Codim<0>::Entity TargetEntity;

  typedef DS::Functions::Expression< SourceEntity, typename SourceGrid::ctype,
      SourceGrid::dimension, typename SourceGrid::ctype, 1 > ScalarFunctionType;
  ScalarFunctionType scalar_f("x", "x[0] + x[1]");
  auto wrapped = femFunctionAdapter(scalar_f);
  Dune::LagrangeInterpolation<typename SourceTraits::DiscreteFunction>::apply(wrapped, source_df);

  DSC_PROFILER.startTiming("HierarchicSearchStrategy");
  DS::HeterogenousProjection<Stuff::HierarchicSearchStrategy>::project(source_df, target_df);
  DSC_PROFILER.stopTiming("HierarchicSearchStrategy");

  DSC_PROFILER.startTiming("EntityInlevelSearch ST");
  DS::HeterogenousProjection<DSG::EntityInlevelSearch>::project(source_df, target_df);
  DSC_PROFILER.stopTiming("EntityInlevelSearch ST");
  vtk_out(source_df);
  vtk_out(target_df);

  if(!do_lagrange)
    return;

  // below code uses dune-fem's entity search and therefore cannot handle differently sized domains
  DiscretefunctionShroud<typename SourceTraits::DiscreteFunction> shrouded_source_df(source_df);
  DSC_PROFILER.startTiming("FemProjection ST");
  Dune::LagrangeInterpolation<typename TargetTraits::DiscreteFunction>::apply(shrouded_source_df, fem_target_df);
  DSC_PROFILER.stopTiming("FemProjection ST");
  vtk_out(fem_target_df);

  DiscretefunctionShroud<typename TargetTraits::DiscreteFunction> shrouded_target_df(target_df);
  DSC_PROFILER.startTiming("FemProjection TS");
  Dune::LagrangeInterpolation<typename SourceTraits::DiscreteFunction>::apply(shrouded_target_df, source_df);
  DSC_PROFILER.stopTiming("FemProjection TS");
}

void ptest(const int macro_elements = 4, const int target_factor = 2) {
  auto source_cube = DS::GridProviderCube<SourceGrid>(0,1,macro_elements).grid();
  auto target_cube = DS::GridProviderCube<TargetGrid>(0,1,macro_elements*target_factor).grid();
  source_cube->globalRefine(2);
  target_cube->globalRefine(2*target_factor);
  ptest(source_cube, target_cube, true);
  ptest(target_cube, source_cube, true);
  source_cube = Stuff::GridProviderCube<SourceGrid>(0.25,0.75,macro_elements).grid();
  source_cube->globalRefine(2);
  ptest(source_cube, target_cube, false);
  ptest(target_cube, source_cube, false);
}

TEST(Projection, Cubes){
  const int rf = DSC_CONFIG_GET("rf", 1);
  const int runs = DSC_CONFIG_GET("runs", 2);
  DSC_PROFILER.reset(runs);
  for (auto i : DSC::valueRange(runs)) {
    ptest(std::pow(2, i+1), rf);
    DSC_PROFILER.nextRun();
  }
  DSC_PROFILER.outputTimingsAll();
}

#endif //#if HAVE_DUNE_FEM

#endif // #if HAVE_DUNE_GRID

int main(int argc, char** argv)
{
  test_init(argc, argv);
  return RUN_ALL_TESTS();
}

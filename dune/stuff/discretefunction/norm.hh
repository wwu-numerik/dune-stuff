#ifndef DUNE_STUFF_DISCRETEFUNCTION_NORM_HH
#define DUNE_STUFF_DISCRETEFUNCTION_NORM_HH

#ifdef HAVE_CMAKE_CONFIG
#include "cmake_config.h"
#else
#include "config.h"
#endif // HAVE_CMAKE_CONFIG

// dune-geometry
#if HAVE_DUNE_GEOMETRY
#include <dune/geometry/quadraturerules.hh>

#if HAVE_DUNE_DETAILED_DISCRETIZATIONS

#include <dune/grid/part/interface.hh>
#include <dune/detailed/discretizations/discretefunction/local.hh>
#include <dune/detailed/discretizations/discretefunction/default.hh>
#include <dune/detailed/discretizations/discretefunction/multiscale.hh>
#include <dune/stuff/functions/interfaces.hh>

namespace Dune {
namespace Stuff {
namespace DiscreteFunction {

class Norm {
public:

template< class GridPartTraits, class DomainFieldType, int dimDomain, class RangeFieldType, int dimRange >
static RangeFieldType L2_squared(const Dune::grid::Part::Interface< GridPartTraits >& gridPart,
                                   const Dune::Stuff::Function::Interface< DomainFieldType, dimDomain, RangeFieldType, dimRange >& function,
                                   const unsigned int functionOrder)
{
  // preparations
  typedef typename Dune::grid::Part::Interface< GridPartTraits > GridPartType;
  typedef typename Dune::Stuff::Function::Interface< DomainFieldType, dimDomain, RangeFieldType, dimRange > FunctionType;
  typedef typename FunctionType::DomainType DomainType;
  typedef typename FunctionType::RangeType RangeType;
  RangeFieldType ret(0);
  RangeType tmp(0);
  // walk the grid
  for (typename GridPartType::template Codim< 0 >::IteratorType entityIt = gridPart.template begin< 0 >();
       entityIt != gridPart.template end< 0 >();
       ++entityIt) {
    // quadrature
    typedef typename GridPartType::template Codim< 0 >::EntityType EntityType;
    const EntityType& entity = *entityIt;
    typedef Dune::QuadratureRules< RangeFieldType, EntityType::mydimension > Quadrature;
    typedef Dune::QuadratureRule< RangeFieldType, EntityType::mydimension > QuadratureType;
    const unsigned int order = functionOrder + functionOrder;
    const QuadratureType& quadrature = Quadrature::rule(entity.type(), 2*order + 1);
    // loop over all quadrature points
    for (const auto& quadPoint : quadrature) {
      // global coordinate
      const DomainType xLocal = quadPoint.position();
      const DomainType xGlobal = entity.geometry().global(xLocal);
      // integration factors
      const RangeFieldType integrationFactor = entity.geometry().integrationElement(xLocal);
      const RangeFieldType quadratureWeight = quadPoint.weight();
      // evaluate
      function.evaluate(xGlobal, tmp);
      // compute
      ret += integrationFactor*quadratureWeight*(tmp*tmp);
      // clear
      tmp = 0;
    } // loop over all quadrature points
  } // walk the grid
  return ret;
} // ... L2_squared(...)

template< class GridPartTraits, class DomainFieldType, int dimDomain, class RangeFieldType, int dimRange >
static RangeFieldType L2(const Dune::grid::Part::Interface< GridPartTraits >& gridPart,
                  const Dune::Stuff::Function::Interface< DomainFieldType, dimDomain, RangeFieldType, dimRange >& function,
                  const unsigned int functionOrder)
{
  return std::sqrt(L2_squared(gridPart, function, functionOrder));
}

template< class DiscreteFunctionType >
static typename DiscreteFunctionType::RangeFieldType L2_squared(const Dune::Detailed::Discretizations::DiscreteFunction::LocalConst< DiscreteFunctionType >& localFunction)
{
  // preparations
  typedef typename DiscreteFunctionType::DomainType DomainType;
  typedef typename DiscreteFunctionType::RangeType RangeType;
  typedef typename DiscreteFunctionType::RangeFieldType RangeFieldType;
  typedef typename Dune::Detailed::Discretizations::DiscreteFunction::LocalConst< DiscreteFunctionType > LocalFunctionType;
  typedef typename LocalFunctionType::EntityType EntityType;
  RangeFieldType ret(0);
  RangeType tmp(0);
  // quadrature
  const EntityType& entity = localFunction.entity();
  typedef Dune::QuadratureRules< RangeFieldType, EntityType::mydimension > Quadrature;
  typedef Dune::QuadratureRule< RangeFieldType, EntityType::mydimension > QuadratureType;
  const unsigned int order = localFunction.order() + localFunction.order();
  const QuadratureType& quadrature = Quadrature::rule(entity.type(), 2*order + 1);
  // loop over all quadrature points
  for (const auto& quadPoint : quadrature) {
    // local coordinate
    const DomainType x = quadPoint.position();
    // integration factors
    const RangeFieldType integrationFactor = entity.geometry().integrationElement(x);
    const RangeFieldType quadratureWeight = quadPoint.weight();
    // evaluate
    localFunction.evaluate(x, tmp);
    // compute
    ret += integrationFactor*quadratureWeight*(tmp*tmp);
    // clear
    tmp = 0;
  } // loop over all quadrature points
  return ret;
} // ... L2_squared(...)

template< class DiscreteFunctionType >
static typename DiscreteFunctionType::RangeFieldType L2(const Dune::Detailed::Discretizations::DiscreteFunction::LocalConst< DiscreteFunctionType >& localFunction)
{
  return std::sqrt(L2_squared(localFunction));
}

template< class DiscreteFunctionSpaceType, class VectorBackendType >
static typename DiscreteFunctionSpaceType::RangeFieldType L2_squared(const Dune::Detailed::Discretizations::DiscreteFunction::Default< DiscreteFunctionSpaceType, VectorBackendType >& discreteFunction)
{
  // preparations
  typedef typename DiscreteFunctionSpaceType::RangeFieldType RangeFieldType;
  typedef typename Dune::Detailed::Discretizations::DiscreteFunction::Default< DiscreteFunctionSpaceType, VectorBackendType > DiscreteFunctionType;
  RangeFieldType ret(0);
  // walk the grid
  typedef typename DiscreteFunctionSpaceType::GridPartType GridPartType;
  const GridPartType& gridPart = discreteFunction.space().gridPart();
  for (typename GridPartType::template Codim< 0 >::IteratorType entityIt = gridPart.template begin< 0 >();
       entityIt != gridPart.template end< 0 >();
       ++entityIt) {
    // local function
    const typename GridPartType::template Codim< 0 >::EntityType& entity = *entityIt;
    const typename DiscreteFunctionType::ConstLocalFunctionType localFunction = discreteFunction.localFunction(entity);
    // add local norm
    ret += L2_squared(localFunction);
  } // walk the grid
  return ret;
} // ... L2_squared(...)

template< class DiscreteFunctionSpaceType, class VectorBackendType >
static typename DiscreteFunctionSpaceType::RangeFieldType L2(const Dune::Detailed::Discretizations::DiscreteFunction::Default< DiscreteFunctionSpaceType, VectorBackendType >& discreteFunction)
{
  return std::sqrt(L2_squared(discreteFunction));
}

template< class MsGridType, class LocalDiscreteFunctionType >
static typename LocalDiscreteFunctionType::RangeFieldType L2_squared(const Dune::Detailed::Discretizations::DiscreteFunction::Multiscale< MsGridType, LocalDiscreteFunctionType >& multiscaleDiscreteFunction)
{
  // preparations
  typedef typename LocalDiscreteFunctionType::RangeFieldType RangeFieldType;
  typedef typename Dune::Detailed::Discretizations::DiscreteFunction::Multiscale< MsGridType, LocalDiscreteFunctionType > MultiscaleDiscreteFunctionType;
  RangeFieldType ret(0);
  // walk the grid
  typedef typename MsGridType::GlobalGridPartType GridPartType;
  const GridPartType& gridPart = *(multiscaleDiscreteFunction.msGrid().globalGridPart());
  for (typename GridPartType::template Codim< 0 >::IteratorType entityIt = gridPart.template begin< 0 >();
       entityIt != gridPart.template end< 0 >();
       ++entityIt) {
    // local function
    const typename GridPartType::template Codim< 0 >::EntityType& entity = *entityIt;
    const typename MultiscaleDiscreteFunctionType::ConstLocalFunctionType localFunction = multiscaleDiscreteFunction.localFunction(entity);
    // add local norm
    ret += L2_squared(localFunction);
  } // walk the grid
  return ret;
} // ... L2_squared(...)

template< class MsGridType, class LocalDiscreteFunctionType >
static typename LocalDiscreteFunctionType::RangeFieldType L2(const Dune::Detailed::Discretizations::DiscreteFunction::Multiscale< MsGridType, LocalDiscreteFunctionType >& multiscaleDiscreteFunction)
{
  return std::sqrt(L2_squared(multiscaleDiscreteFunction));
}

template< class DomainFieldType, int dimDomain, class RangeFieldType, int dimRange, class DiscreteFunctionSpaceType, class VectorBackendType >
static RangeFieldType L2_difference_squared(const Dune::Stuff::Function::Interface< DomainFieldType, dimDomain, RangeFieldType, dimRange >& function,
                                     const unsigned int functionOrder,
                                     const Dune::Detailed::Discretizations::DiscreteFunction::Default< DiscreteFunctionSpaceType, VectorBackendType >& discreteFunction)
{
  // preparations
  typedef typename DiscreteFunctionSpaceType::RangeType RangeType;
  typedef typename DiscreteFunctionSpaceType::DomainType DomainType;
  typedef typename Dune::Stuff::Function::Interface< DomainFieldType, dimDomain, RangeFieldType, dimRange > FunctionType;
  typedef typename Dune::Detailed::Discretizations::DiscreteFunction::Default< DiscreteFunctionSpaceType, VectorBackendType > DiscreteFunctionType;
  RangeFieldType ret(0);
  RangeType val_1(0);
  RangeType val_2(0);
  RangeType diff(0);
  // walk the grid
  typedef typename DiscreteFunctionSpaceType::GridPartType GridPartType;
  const GridPartType& gridPart = discreteFunction.space().gridPart();
  for (typename GridPartType::template Codim< 0 >::IteratorType entityIt = gridPart.template begin< 0 >();
       entityIt != gridPart.template end< 0 >();
       ++entityIt) {
    // local function
    typedef typename GridPartType::template Codim< 0 >::EntityType EntityType;
    const EntityType& entity = *entityIt;
    const typename DiscreteFunctionType::ConstLocalFunctionType localFunction = discreteFunction.localFunction(entity);
    // compute local difference
    typedef Dune::QuadratureRules< RangeFieldType, EntityType::mydimension > Quadrature;
    typedef Dune::QuadratureRule< RangeFieldType, EntityType::mydimension > QuadratureType;
    const unsigned int order = 2*std::max(int(functionOrder), localFunction.order());
    const QuadratureType& quadrature = Quadrature::rule(entity.type(), 2*order + 1);
    // loop over all quadrature points
    for (typename QuadratureType::const_iterator quadPoint = quadrature.begin();
         quadPoint != quadrature.end();
         ++quadPoint) {
      // global coordinate
      const DomainType xLocal = quadPoint->position();
      const DomainType xGlobal = entity.geometry().global(xLocal);
      // integration factors
      const RangeFieldType integrationFactor = entity.geometry().integrationElement(xLocal);
      const RangeFieldType quadratureWeight = quadPoint->weight();
      // evaluate
      function.evaluate(xGlobal, val_1);
      localFunction.evaluate(xLocal, val_2);
      diff = val_1 - val_2;
      // compute
      ret += integrationFactor*quadratureWeight*(diff*diff);
      // clear
      val_1 = 0;
      val_2 = 0;
      diff  = 0;
    } // loop over all quadrature points
  } // walk the grid
  return ret;
} // ... L2_difference_squared(...)

template< class DomainFieldType, int dimDomain, class RangeFieldType, int dimRange, class DiscreteFunctionSpaceType, class VectorBackendType >
static RangeFieldType L2_difference(const Dune::Stuff::Function::Interface< DomainFieldType, dimDomain, RangeFieldType, dimRange >& function,
                             const unsigned int functionOrder,
                             const Dune::Detailed::Discretizations::DiscreteFunction::Default< DiscreteFunctionSpaceType, VectorBackendType >& discreteFunction)
{
  return std::sqrt(L2_difference_squared(function, functionOrder, discreteFunction));
}

template< class DomainFieldType, int dimDomain, class RangeFieldType, int dimRange, class MsGridType, class LocalDiscreteFunctionType >
static RangeFieldType L2_difference_squared(const Dune::Stuff::Function::Interface< DomainFieldType, dimDomain, RangeFieldType, dimRange >& function,
                                     const unsigned int functionOrder,
                                     const Dune::Detailed::Discretizations::DiscreteFunction::Multiscale< MsGridType, LocalDiscreteFunctionType >& multiscaleDiscreteFunction)
{
  // preparations
  typedef typename LocalDiscreteFunctionType::RangeType RangeType;
  typedef typename LocalDiscreteFunctionType::DomainType DomainType;
  typedef typename Dune::Stuff::Function::Interface< DomainFieldType, dimDomain, RangeFieldType, dimRange > FunctionType;
  typedef typename Dune::Detailed::Discretizations::DiscreteFunction::Multiscale< MsGridType, LocalDiscreteFunctionType > MultiscaleDiscreteFunctionType;
  RangeFieldType ret(0);
  RangeType val_1(0);
  RangeType val_2(0);
  RangeType diff(0);
  // walk the grid
  typedef typename MsGridType::GlobalGridPartType GridPartType;
  const GridPartType& gridPart = *(multiscaleDiscreteFunction.msGrid().globalGridPart());
  for (typename GridPartType::template Codim< 0 >::IteratorType entityIt = gridPart.template begin< 0 >();
       entityIt != gridPart.template end< 0 >();
       ++entityIt) {
    // local function
    typedef typename GridPartType::template Codim< 0 >::EntityType EntityType;
    const EntityType& entity = *entityIt;
    const typename MultiscaleDiscreteFunctionType::ConstLocalFunctionType localFunction = multiscaleDiscreteFunction.localFunction(entity);
    // compute local difference
    typedef Dune::QuadratureRules< RangeFieldType, EntityType::mydimension > Quadrature;
    typedef Dune::QuadratureRule< RangeFieldType, EntityType::mydimension > QuadratureType;
    const unsigned int order = 2*std::max(int(functionOrder), localFunction.order());
    const QuadratureType& quadrature = Quadrature::rule(entity.type(), 2*order + 1);
    // loop over all quadrature points
    for (typename QuadratureType::const_iterator quadPoint = quadrature.begin();
         quadPoint != quadrature.end();
         ++quadPoint) {
      // global coordinate
      const DomainType xLocal = quadPoint->position();
      const DomainType xGlobal = entity.geometry().global(xLocal);
      // integration factors
      const RangeFieldType integrationFactor = entity.geometry().integrationElement(xLocal);
      const RangeFieldType quadratureWeight = quadPoint->weight();
      // evaluate
      function.evaluate(xGlobal, val_1);
      localFunction.evaluate(xLocal, val_2);
      diff = val_1 - val_2;
      // compute
      ret += integrationFactor*quadratureWeight*(diff*diff);
      // clear
      val_1 = 0;
      val_2 = 0;
      diff  = 0;
    } // loop over all quadrature points
  } // walk the grid
  return ret;
} // ... L2_difference_squared(...)

template< class DomainFieldType, int dimDomain, class RangeFieldType, int dimRange, class MsGridType, class LocalDiscreteFunctionType >
static RangeFieldType L2_difference(const Dune::Stuff::Function::Interface< DomainFieldType, dimDomain, RangeFieldType, dimRange >& function,
                             const unsigned int functionOrder,
                             const Dune::Detailed::Discretizations::DiscreteFunction::Multiscale< MsGridType, LocalDiscreteFunctionType >& multiscaleDiscreteFunction)
{
  return std::sqrt(L2_difference_squared(function, functionOrder, multiscaleDiscreteFunction));
}

template< class DiscreteFunctionType_1, class DiscreteFunctionType_2 >
static typename DiscreteFunctionType_1::RangeFieldType L2_difference_squared(const Dune::Detailed::Discretizations::DiscreteFunction::LocalConst< DiscreteFunctionType_1 >& localFunction_1,
                                                                      const Dune::Detailed::Discretizations::DiscreteFunction::LocalConst< DiscreteFunctionType_2 >& localFunction_2)
{
  // preparations
  typedef typename DiscreteFunctionType_1::DomainType DomainType;
  typedef typename DiscreteFunctionType_1::RangeType RangeType;
  typedef typename DiscreteFunctionType_1::RangeFieldType RangeFieldType;
  typedef typename Dune::Detailed::Discretizations::DiscreteFunction::LocalConst< DiscreteFunctionType_1 > LocalFunctionType;
  typedef typename LocalFunctionType::EntityType EntityType;
  RangeFieldType ret(0);
  RangeType val_1(0);
  RangeType val_2(0);
  RangeType diff(0);
  // quadrature
  const EntityType& entity = localFunction_1.entity();
  typedef Dune::QuadratureRules< RangeFieldType, EntityType::mydimension > Quadrature;
  typedef Dune::QuadratureRule< RangeFieldType, EntityType::mydimension > QuadratureType;
  const unsigned int order = 2*std::max(localFunction_1.order(), localFunction_2.order());
  const QuadratureType& quadrature = Quadrature::rule(entity.type(), 2*order + 1);
  // loop over all quadrature points
  for (typename QuadratureType::const_iterator quadPoint = quadrature.begin();
       quadPoint != quadrature.end();
       ++quadPoint) {
    // local coordinate
    const DomainType x = quadPoint->position();
    // integration factors
    const RangeFieldType integrationFactor = entity.geometry().integrationElement(x);
    const RangeFieldType quadratureWeight = quadPoint->weight();
    // evaluate
    localFunction_1.evaluate(x, val_1);
    localFunction_2.evaluate(x, val_2);
    diff = val_1 - val_2;
    // compute
    ret += integrationFactor*quadratureWeight*(diff*diff);
    // clear
    val_1 = 0;
    val_2 = 0;
    diff  = 0;
  } // loop over all quadrature points
  return ret;
} // ... L2_difference_squared(...)

template< class DiscreteFunctionType_1, class DiscreteFunctionType_2 >
static typename DiscreteFunctionType_1::RangeFieldType L2_difference(const Dune::Detailed::Discretizations::DiscreteFunction::LocalConst< DiscreteFunctionType_1 >& localFunction_1,
                                                              const Dune::Detailed::Discretizations::DiscreteFunction::LocalConst< DiscreteFunctionType_2 >& localFunction_2)
{
  return std::sqrt(L2_difference_squared(localFunction_1, localFunction_2));
}

template< class DiscreteFunctionSpaceType, class VectorBackendType, class MsGridType, class LocalDiscreteFunctionType >
static typename DiscreteFunctionSpaceType::RangeFieldType L2_difference_squared(const Dune::Detailed::Discretizations::DiscreteFunction::Default< DiscreteFunctionSpaceType, VectorBackendType >& discreteFunction,
                                                                         const Dune::Detailed::Discretizations::DiscreteFunction::Multiscale< MsGridType, LocalDiscreteFunctionType >& multiscaleDiscreteFunction)
{
//  // test for "equal" grid parts
//  assert(discreteFunction.space().gridPart().size(0) == multiscaleDiscreteFunction.msGrid().globalGridPart()->size(0)
//         && "The GridParts need to have the same size!");
  // preparations
  typedef typename DiscreteFunctionSpaceType::RangeFieldType RangeFieldType;
  typedef typename Dune::Detailed::Discretizations::DiscreteFunction::Default< DiscreteFunctionSpaceType, VectorBackendType > DiscreteFunctionType;
  typedef typename Dune::Detailed::Discretizations::DiscreteFunction::Multiscale< MsGridType, LocalDiscreteFunctionType > MultiscaleDiscreteFunctionType;
  RangeFieldType ret(0);
  // walk the grid
  typedef typename DiscreteFunctionSpaceType::GridPartType GridPartType;
  const GridPartType& gridPart = discreteFunction.space().gridPart();
  for (typename GridPartType::template Codim< 0 >::IteratorType entityIt = gridPart.template begin< 0 >();
       entityIt != gridPart.template end< 0 >();
       ++entityIt) {
    // local function
    const typename GridPartType::template Codim< 0 >::EntityType& entity = *entityIt;
    const typename DiscreteFunctionType::ConstLocalFunctionType           localFunction_1 = discreteFunction.localFunction(entity);
    const typename MultiscaleDiscreteFunctionType::ConstLocalFunctionType localFunction_2 = multiscaleDiscreteFunction.localFunction(entity);
    // add local difference
    ret += L2_difference_squared(localFunction_1, localFunction_2);
  } // walk the grid
  return ret;
} // ... L2_difference_squared(...)

template< class DiscreteFunctionSpaceType, class VectorBackendType, class MsGridType, class LocalDiscreteFunctionType >
static typename DiscreteFunctionSpaceType::RangeFieldType L2_difference(const Dune::Detailed::Discretizations::DiscreteFunction::Default< DiscreteFunctionSpaceType, VectorBackendType >& discreteFunction,
                                                                 const Dune::Detailed::Discretizations::DiscreteFunction::Multiscale< MsGridType, LocalDiscreteFunctionType >& multiscaleDiscreteFunction)
{
  return std::sqrt(L2_difference_squared(discreteFunction, multiscaleDiscreteFunction));
}

template< class DiscreteFunctionType >
static typename DiscreteFunctionType::RangeFieldType h1_squared(const Dune::Detailed::Discretizations::DiscreteFunction::LocalConst< DiscreteFunctionType >& localFunction)
{
  // preparations
  typedef typename DiscreteFunctionType::DomainType DomainType;
  typedef typename DiscreteFunctionType::JacobianRangeType JacobianRangeType;
  typedef typename DiscreteFunctionType::RangeFieldType RangeFieldType;
  typedef typename Dune::Detailed::Discretizations::DiscreteFunction::LocalConst< DiscreteFunctionType > LocalFunctionType;
  typedef typename LocalFunctionType::EntityType EntityType;
  RangeFieldType ret(0);
  JacobianRangeType tmp(0);
  // quadrature
  const EntityType& entity = localFunction.entity();
  typedef Dune::QuadratureRules< RangeFieldType, EntityType::mydimension > Quadrature;
  typedef Dune::QuadratureRule< RangeFieldType, EntityType::mydimension > QuadratureType;
  const unsigned int order = std::max((localFunction.order() - 1) + (localFunction.order() - 1),
                                      0);
  const QuadratureType& quadrature = Quadrature::rule(entity.type(), 2*order + 1);
  // loop over all quadrature points
  for (typename QuadratureType::const_iterator quadPoint = quadrature.begin();
       quadPoint != quadrature.end();
       ++quadPoint) {
    // local coordinate
    const DomainType x = quadPoint->position();
    // integration factors
    const RangeFieldType integrationFactor = entity.geometry().integrationElement(x);
    const RangeFieldType quadratureWeight = quadPoint->weight();
    // evaluate
    localFunction.jacobian(x, tmp);
    // compute
    for (int d = 0; d < DiscreteFunctionType::dimRange; ++d) {
      ret += integrationFactor*quadratureWeight*(tmp[d]*tmp[d]);
    }
    // clear
    tmp = 0;
  } // loop over all quadrature points
  return ret;
} // ... h1_squared(...)

template< class DiscreteFunctionType >
static typename DiscreteFunctionType::RangeFieldType h1(const Dune::Detailed::Discretizations::DiscreteFunction::LocalConst< DiscreteFunctionType >& localFunction)
{
  return std::sqrt(h1_squared(localFunction));
}

template< class DiscreteFunctionSpaceType, class VectorBackendType >
static typename DiscreteFunctionSpaceType::RangeFieldType h1_squared(const Dune::Detailed::Discretizations::DiscreteFunction::Default< DiscreteFunctionSpaceType, VectorBackendType >& discreteFunction)
{
  // preparations
  typedef typename DiscreteFunctionSpaceType::RangeFieldType RangeFieldType;
  typedef typename Dune::Detailed::Discretizations::DiscreteFunction::Default< DiscreteFunctionSpaceType, VectorBackendType > DiscreteFunctionType;
  RangeFieldType ret(0);
  // walk the grid
  typedef typename DiscreteFunctionSpaceType::GridPartType GridPartType;
  const GridPartType& gridPart = discreteFunction.space().gridPart();
  for (typename GridPartType::template Codim< 0 >::IteratorType entityIt = gridPart.template begin< 0 >();
       entityIt != gridPart.template end< 0 >();
       ++entityIt) {
    // local function
    const typename GridPartType::template Codim< 0 >::EntityType& entity = *entityIt;
    const typename DiscreteFunctionType::ConstLocalFunctionType localFunction = discreteFunction.localFunction(entity);
    // add local norm
    ret += h1_squared(localFunction);
  } // walk the grid
  return ret;
} // ... h1_squared(...)

template< class DiscreteFunctionSpaceType, class VectorBackendType >
static typename DiscreteFunctionSpaceType::RangeFieldType h1(const Dune::Detailed::Discretizations::DiscreteFunction::Default< DiscreteFunctionSpaceType, VectorBackendType >& discreteFunction)
{
  return std::sqrt(h1_squared(discreteFunction));
}

template< class MsGridType, class LocalDiscreteFunctionType >
static typename LocalDiscreteFunctionType::RangeFieldType h1_squared(const Dune::Detailed::Discretizations::DiscreteFunction::Multiscale< MsGridType, LocalDiscreteFunctionType >& multiscaleDiscreteFunction)
{
  // preparations
  typedef typename LocalDiscreteFunctionType::RangeFieldType RangeFieldType;
  typedef typename Dune::Detailed::Discretizations::DiscreteFunction::Multiscale< MsGridType, LocalDiscreteFunctionType > MultiscaleDiscreteFunctionType;
  RangeFieldType ret(0);
  // walk the grid
  typedef typename MsGridType::GlobalGridPartType GridPartType;
  const GridPartType& gridPart = *(multiscaleDiscreteFunction.msGrid().globalGridPart());
  for (typename GridPartType::template Codim< 0 >::IteratorType entityIt = gridPart.template begin< 0 >();
       entityIt != gridPart.template end< 0 >();
       ++entityIt) {
    // local function
    const typename GridPartType::template Codim< 0 >::EntityType& entity = *entityIt;
    const typename MultiscaleDiscreteFunctionType::ConstLocalFunctionType localFunction = multiscaleDiscreteFunction.localFunction(entity);
    // add local norm
    ret += h1_squared(localFunction);
  } // walk the grid
  return ret;
} // ... h1_squared(...)

template< class MsGridType, class LocalDiscreteFunctionType >
static typename LocalDiscreteFunctionType::RangeFieldType h1(const Dune::Detailed::Discretizations::DiscreteFunction::Multiscale< MsGridType, LocalDiscreteFunctionType >& multiscaleDiscreteFunction)
{
  return std::sqrt(h1_squared(multiscaleDiscreteFunction));
}

template< class DiscreteFunctionType_1, class DiscreteFunctionType_2 >
static typename DiscreteFunctionType_1::RangeFieldType h1_difference_squared(const Dune::Detailed::Discretizations::DiscreteFunction::LocalConst< DiscreteFunctionType_1 >& localFunction_1,
                                                                      const Dune::Detailed::Discretizations::DiscreteFunction::LocalConst< DiscreteFunctionType_2 >& localFunction_2)
{
  // preparations
  typedef typename DiscreteFunctionType_1::DomainType DomainType;
  typedef typename DiscreteFunctionType_1::JacobianRangeType JacobianRangeType;
  typedef typename DiscreteFunctionType_1::RangeFieldType RangeFieldType;
  typedef typename Dune::Detailed::Discretizations::DiscreteFunction::LocalConst< DiscreteFunctionType_1 > LocalFunctionType;
  typedef typename LocalFunctionType::EntityType EntityType;
  RangeFieldType ret(0);
  JacobianRangeType val_1(0);
  JacobianRangeType val_2(0);
  JacobianRangeType diff(0);
  // quadrature
  const EntityType& entity = localFunction_1.entity();
  typedef Dune::QuadratureRules< RangeFieldType, EntityType::mydimension > Quadrature;
  typedef Dune::QuadratureRule< RangeFieldType, EntityType::mydimension > QuadratureType;
  const unsigned int order = localFunction_1.order() + localFunction_2.order();
  const QuadratureType& quadrature = Quadrature::rule(entity.type(), 2*order + 1);
  // loop over all quadrature points
  for (typename QuadratureType::const_iterator quadPoint = quadrature.begin();
       quadPoint != quadrature.end();
       ++quadPoint) {
    // local coordinate
    const DomainType x = quadPoint->position();
    // integration factors
    const RangeFieldType integrationFactor = entity.geometry().integrationElement(x);
    const RangeFieldType quadratureWeight = quadPoint->weight();
    // evaluate
    localFunction_1.jacobian(x, val_1);
    localFunction_2.jacobian(x, val_2);
    diff = val_1 - val_2;
    // compute
    for (int d = 0; d < DiscreteFunctionType_1::dimRange; ++d) {
      ret += integrationFactor*quadratureWeight*(diff[d]*diff[d]);
    }
    // clear
    val_1 = 0;
    val_2 = 0;
    diff  = 0;
  } // loop over all quadrature points
  return ret;
} // ... L2_difference_squared(...)

template< class DiscreteFunctionType_1, class DiscreteFunctionType_2 >
static typename DiscreteFunctionType_1::RangeFieldType h1_difference(const Dune::Detailed::Discretizations::DiscreteFunction::LocalConst< DiscreteFunctionType_1 >& localFunction_1,
                                                              const Dune::Detailed::Discretizations::DiscreteFunction::LocalConst< DiscreteFunctionType_2 >& localFunction_2)
{
  return std::sqrt(h1_difference_squared(localFunction_1, localFunction_2));
}

template< class DiscreteFunctionSpaceType, class VectorBackendType, class MsGridType, class LocalDiscreteFunctionType >
static typename DiscreteFunctionSpaceType::RangeFieldType h1_difference_squared(const Dune::Detailed::Discretizations::DiscreteFunction::Default< DiscreteFunctionSpaceType, VectorBackendType >& discreteFunction,
                                                                         const Dune::Detailed::Discretizations::DiscreteFunction::Multiscale< MsGridType, LocalDiscreteFunctionType >& multiscaleDiscreteFunction)
{
//  // test for "equal" grid parts
//  assert(discreteFunction.space().gridPart().size(0) == multiscaleDiscreteFunction.msGrid().globalGridPart()->size(0)
//         && "The GridParts need to have the same size!");
  // preparations
  typedef typename DiscreteFunctionSpaceType::RangeFieldType RangeFieldType;
  typedef typename Dune::Detailed::Discretizations::DiscreteFunction::Default< DiscreteFunctionSpaceType, VectorBackendType > DiscreteFunctionType;
  typedef typename Dune::Detailed::Discretizations::DiscreteFunction::Multiscale< MsGridType, LocalDiscreteFunctionType > MultiscaleDiscreteFunctionType;
  RangeFieldType ret(0);
  // walk the grid
  typedef typename DiscreteFunctionSpaceType::GridPartType GridPartType;
  const GridPartType& gridPart = discreteFunction.space().gridPart();
  for (typename GridPartType::template Codim< 0 >::IteratorType entityIt = gridPart.template begin< 0 >();
       entityIt != gridPart.template end< 0 >();
       ++entityIt) {
    // local function
    const typename GridPartType::template Codim< 0 >::EntityType& entity = *entityIt;
    const typename DiscreteFunctionType::ConstLocalFunctionType           localFunction_1 = discreteFunction.localFunction(entity);
    const typename MultiscaleDiscreteFunctionType::ConstLocalFunctionType localFunction_2 = multiscaleDiscreteFunction.localFunction(entity);
    // add local difference
    ret += h1_difference_squared(localFunction_1, localFunction_2);
  } // walk the grid
  return ret;
} // ... h1_difference_squared(...)

template< class DiscreteFunctionSpaceType, class VectorBackendType, class MsGridType, class LocalDiscreteFunctionType >
static typename DiscreteFunctionSpaceType::RangeFieldType h1_difference(const Dune::Detailed::Discretizations::DiscreteFunction::Default< DiscreteFunctionSpaceType, VectorBackendType >& discreteFunction,
                                                                 const Dune::Detailed::Discretizations::DiscreteFunction::Multiscale< MsGridType, LocalDiscreteFunctionType >& multiscaleDiscreteFunction)
{
  return std::sqrt(h1_difference_squared(discreteFunction, multiscaleDiscreteFunction));
}

}; // class Norm

} // namespace DiscreteFunction
} // namespace Stuff
} // namespace Dune

#endif // HAVE_DUNE_GEOMETRY
#endif // HAVE_DUNE_DETAILED_DISCRETIZATIONS
#endif // DUNE_STUFF_DISCRETEFUNCTION_NORM_HH

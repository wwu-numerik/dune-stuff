// This file is part of the dune-stuff project:
//   http://users.dune-project.org/projects/dune-stuff
// Copyright holders: Rene Milk, Felix Schindler
// License: BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)
//
// Contributors: Sven Kaulmann

#ifndef DUNE_STUFF_INTEGRALS_HH
#define DUNE_STUFF_INTEGRALS_HH

#if HAVE_DUNE_FEM

#include <dune/stuff/fem/localmassmatrix.hh>
#include <dune/stuff/common/ranges.hh>
#include <dune/fem/quadrature/cachingquadrature.hh>
#include <dune/fem/function/common/discretefunction.hh>

#include <dune/stuff/fem/namespace.hh>

namespace Dune {
namespace Stuff {
namespace Fem {

/** Generate and return a Dune::Fem::CachingQuadrature on a codim-0 entity of the grid.
 *
 *  @param[in] entity The codim-0 entity that will be used as domain for the quadrature
 *  @param[in] space The discrete function space that hosts function to be integrated with this quadrature
 *  @param[in] order The desired order for the quadrature
 *  @return Returns a caching quadrature of order order (or 2*space.order()+2 if no order was provided) on the entity.
 */
template <class SpaceTraits, class GridImp, template <int, int, class> class EntityImp>
Dune::Fem::CachingQuadrature<typename SpaceTraits::GridPartType, 0>
make_quadrature(const Dune::Entity<0, GridImp::dimension, GridImp, EntityImp>& entity,
                const Dune::Fem::DiscreteFunctionSpaceInterface<SpaceTraits>& space, int order = -1) {
  order = order > -1 ? order : 2 * space.order() + 2;
  return Dune::Fem::CachingQuadrature<typename SpaceTraits::GridPartType, 0>(entity, order);
}

/** Generate and return a Dune::Fem::CachingQuadrature on a codim-1 entity of the grid.
 *
 *  @param[in] intersection The codim-1 entity that will be used as domain for the quadrature
 *  @param[in] space The discrete function space that hosts function to be integrated with this quadrature
 *  @param[in] order The desired order for the quadrature
 *  @param[in] inside Switch to change from inside quadrature to outside.
 *  @return Returns a caching quadrature of order order (or 2*space.order()+2 if no order was provided) on the entity.
 */
template <class SpaceTraits, class IntersectionImp>
Dune::Fem::CachingQuadrature<typename SpaceTraits::GridPartType, 1> make_quadrature(
    const IntersectionImp& intersection,
    const Dune::Fem::DiscreteFunctionSpaceInterface<SpaceTraits>& space, int order = -1, bool inside = true) {
  order = order > -1 ? order : 2 * space.order() + 2;
  typedef Dune::Fem::CachingQuadrature<typename SpaceTraits::GridPartType, 1> Quad;
  // this const_cast cast is necessary because the gridPart() method in DiscreteFunctionSpaceInterface
  // has no const version
  auto& castedSpace = const_cast<Dune::Fem::DiscreteFunctionSpaceInterface<SpaceTraits>&>(space);
  return Quad(castedSpace.gridPart(), intersection, order, inside ? Quad::INSIDE : Quad::OUTSIDE);
}


/** Compute the integral of a given discrete function on a codim-0 entity and the entity's volume.
*
* @param[in] function The discrete function
* @param[in] entity The entity
* @param[in] order The order for the quadrature
* @return Returns a std::pair containing the integral of function over entity and entity's volume.
*/
template< class FunctionTraits >
std::pair< typename FunctionTraits::RangeType, double >
integralAndVolume(const Dune::Fem::DiscreteFunctionInterface<FunctionTraits>& function,
                  const typename FunctionTraits::DiscreteFunctionSpaceType::EntityType& entity,
                  const int order = -1) {
  typedef typename FunctionTraits::RangeType RangeType;

  const auto& quadrature = make_quadrature(entity, function.space(), order);

  const auto& geometry = entity.geometry();
  const int quadNop = quadrature.nop();

  std::vector<RangeType> evals(quadNop);
  const auto localFunction = function.localFunction(entity);
  localFunction.evaluateQuadrature(quadrature, evals);

  RangeType integral(0.0);
  for (const auto& i : Dune::Stuff::Common::valueRange(quadNop)) {
    const auto factor = quadrature.weight(i) * geometry.integrationElement(quadrature.point(i));
    integral += evals[i]*factor;
  }
  return std::make_pair(integral, entity.geometry().volume());
}

/** Compute the integral of a given discrete function on a codim-1 entity and the entity's volume.
*
* @param[in] function The discrete function
* @param[in] intersection The entity
* @param[in] order The order for the quadrature
* @param[in] inside Bool signifying whether an INSIDE-quadrature shall be used.
* @return Returns a std::pair containing the integral of function over entity and entity's volume.
*/
template< class FunctionTraits >
std::pair< typename FunctionTraits::RangeType, double >
integralAndVolume(const Dune::Fem::DiscreteFunctionInterface<FunctionTraits>& function,
                  const typename FunctionTraits::DiscreteFunctionSpaceType::IntersectionType& intersection,
                  int order = -1,
                  bool inside = true) {
  typedef typename FunctionTraits::RangeType RangeType;

  const auto& quadrature = make_quadrature(intersection, function.space(), order, inside);

  const auto& geometry = intersection.geometry();
  const int quadNop = quadrature.nop();

  const auto entityPtr = (inside) ? intersection.inside() : intersection.outside();
  const auto& entity = *entityPtr;

  std::vector<RangeType> evals(quadNop);
  const auto localFunction = function.localFunction(entity);
  localFunction.evaluateQuadrature(quadrature, evals);

  RangeType integral(0.0);
  for (const auto& i : Dune::Stuff::Common::valueRange(quadNop)) {
    const double factor = quadrature.weight(i) * geometry.integrationElement(quadrature.localPoint(i));
    integral += evals[i]*factor;
  }
  return std::make_pair(integral, intersection.geometry().volume());
}

/** Compute the integral of a given discrete function on the grid part and the grid part's volume.
*
* "The grid part" here refers to the grid part where the function was defined.
*
* @param[in] function The discrete function
* @param[in] order The order for the quadrature
* @return Returns a std::pair containing the integral of function over the grid part and the grid part's volume.
*/
template< class FunctionTraits >
std::pair< typename FunctionTraits::RangeType, double >
integralAndVolume(const Dune::Fem::DiscreteFunctionInterface<FunctionTraits>& function,
                  const int order = -1) {
  typedef typename FunctionTraits::RangeType RangeType;

  RangeType globalIntegral(0.0);
  double globalVolume(0.0);
  // grid run, compute per-element contributions
  for (const auto& entity : function.space()) {
    const auto& localPair = integralAndVolume(function, entity, order);
    globalIntegral += localPair.first;
    globalVolume += localPair.second;
  }

  // for parallel runs: sum up over all processes
  const auto& communication = function.space().gridPart().grid().comm();
  std::pair<RangeType, double> globalPair(communication.sum(globalIntegral), communication.sum(globalVolume));

  return globalPair;
}

/** Compute the mean value of a given discrete function on a codim-0 entity.
*
* @param[in] function The discrete function
* @param[in] entity The entity
* @param[in] order The order for the quadrature
* @return Returns the mean value of the function over the entity.
*/
template< class FunctionTraits >
typename FunctionTraits::RangeType
meanValue(const Dune::Fem::DiscreteFunctionInterface<FunctionTraits>& function,
          const typename FunctionTraits::DiscreteFunctionSpaceType::EntityType& entity,
          const int order = -1) {
  const auto& intAndVol = integralAndVolume(function, entity, order);
  return (intAndVol.first/intAndVol.second);
}

/** Compute the mean value of a given discrete function on a codim-1 entity.
*
* @param[in] function The discrete function
* @param[in] intersection The entity
* @param[in] order The order for the quadrature
* @param[in] inside Bool signifying whether an INSIDE-quadrature shall be used.
* @return Returns the mean value of the function over the intersection.
*/
template< class FunctionTraits >
typename FunctionTraits::RangeType
meanValue(const Dune::Fem::DiscreteFunctionInterface<FunctionTraits>& function,
          const typename FunctionTraits::DiscreteFunctionSpaceType::IntersectionType& intersection,
          const int order = -1,
          const bool inside = true) {
  const auto& intAndVol = integralAndVolume(function, intersection, order, inside);
  return (intAndVol.first/intAndVol.second);
}

/** Compute the mean value of a given discrete function on the grid part.
*
* "The grid part" here refers to the grid part where the function was defined.
*
* @param[in] function The discrete function
* @param[in] order The order for the quadrature
* @return Returns the mean value of the function over the grid part.
*/
template< class FunctionTraits >
typename FunctionTraits::RangeType
meanValue(const Dune::Fem::DiscreteFunctionInterface<FunctionTraits>& function,
          const int order = -1) {
  const auto& intAndVol = integralAndVolume(function, order);
  return (intAndVol.first/intAndVol.second);
}

/** \todo RENE needs to doc me **/
template< class FunctionType, class SpaceTraits >
std::pair< typename FunctionType::RangeType, double >
integralAndVolume(const FunctionType& function,
                  const Dune::Fem::DiscreteFunctionSpaceInterface<SpaceTraits>& space,
                  const int polOrd = -1)
{
//  typedef Dune::Stuff::Fem::LocalMassMatrix< DiscreteFunctionSpaceType, QuadratureType > LocalMassMatrixType;

  typedef typename FunctionType::RangeType RangeType;

  RangeType integral_value(0);
  double total_volume = 0;
  RangeType ret(0.0);

  for (const auto& en : space)
  {
    const auto& geo = en.geometry();
    total_volume += geo.volume();
    const auto& quad = make_quadrature(en, space, polOrd);
    for (const auto& qP : Dune::Stuff::Common::valueRange(quad.nop())) {
      const double intel = quad.weight(qP) * geo.integrationElement( quad.point(qP) );
      // evaluate function
      RangeType dummy;
      const auto xWorld = geo.global( quad.point(qP) );
      function.evaluate(xWorld, dummy);
      ret = dummy;
      ret *= intel;

      integral_value += ret;
    }
  }
  const auto& comm = space.gridPart().grid().comm();
  integral_value = comm.sum(integral_value);
  total_volume = comm.sum(total_volume);
  return std::make_pair(integral_value, total_volume);
} // integralAndVolume

/** \todo RENE needs to doc me **/
template< class FunctionType, class SpaceTraits >
typename FunctionType::RangeType meanValue(const FunctionType& function,
                                           const Dune::Fem::DiscreteFunctionSpaceInterface<SpaceTraits>& space,
                                           const int polOrd = -1) {
  std::pair< typename FunctionType::RangeType, double > pair = integralAndVolume(function, space, polOrd);
  pair.first /= pair.second;
  return pair.first;
}

/** \todo RENE needs to doc me **/
template< class FunctionType, class DiscreteFunctionSpaceType >
double boundaryIntegral(const FunctionType& function,
                        const DiscreteFunctionSpaceType& space,
                        const int polOrd = -1)
{
  typedef typename DiscreteFunctionSpaceType::Traits::GridPartType GridPartType;
  typedef Dune::Fem::CachingQuadrature< GridPartType, 1 > QuadratureType;
  typedef Dune::Stuff::Fem::LocalMassMatrix< DiscreteFunctionSpaceType,
                                  Dune::Fem::CachingQuadrature< GridPartType, 0 > > LocalMassMatrixType;
  double integral_value = 0;
  double total_volume = 0;
  typename DiscreteFunctionSpaceType::RangeType ret(0.0);
  const int quadOrd = std::max(2 * space.order() + 2, polOrd);
  // create local mass matrix object
  LocalMassMatrixType massMatrix(space, quadOrd);
  // check whether geometry mappings are affine or not
  const bool affineMapping = massMatrix.affine();
  for (const auto& entity : space)
  {
    const auto& geo = entity.geometry();
    total_volume += geo.volume();

    const auto intItEnd = space.gridPart().iend(entity);
    for (auto intIt = space.gridPart().ibegin(entity);
         intIt != intItEnd;
         ++intIt)
    {
      if ( !intIt->neighbor() && intIt->boundary() )
      {
        const QuadratureType quad(space.gridPart(),
                                  *intIt,
                                  quadOrd,
                                  QuadratureType::INSIDE);
        const int quadNop = quad.nop();
        for (int qP = 0; qP < quadNop; ++qP)
        {
          const double intel = (affineMapping) ?
                               quad.weight(qP) : // affine case
                               quad.weight(qP) * geo.integrationElement( quad.point(qP) ); // general case

          // evaluate function
          typename DiscreteFunctionSpaceType::RangeType dummy;
          const auto xWorld = geo.global( quad.point(qP) );
          function.evaluate(xWorld, dummy, *intIt);
          ret = dummy;
          ret *= intel;
          integral_value += ret * intIt->unitOuterNormal( quad.localPoint(qP) );
        }
      }
    }
  }
  return integral_value;
} // boundaryIntegral


} // end namespace Fem
} // end namespace Stuff
} // end namespace Dune

#endif // HAVE_DUNE_FEM

#endif // DUNE_STUFF_INTEGRALS_HH

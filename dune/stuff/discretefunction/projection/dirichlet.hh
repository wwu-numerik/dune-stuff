#ifndef DUNE_STUFF_DISCRETEFUNCTION_PROJECTION_DIRICHLET_HH
#define DUNE_STUFF_DISCRETEFUNCTION_PROJECTION_DIRICHLET_HH

// system
#include <vector>

// dune-common
#include <dune/common/shared_ptr.hh>

#ifdef HAVE_DUNE_DETAILED_DISCRETIZATIONS
// dune-detailed-discretizations
#include <dune/detailed/discretizations/discretefunction/default.hh>
#endif // HAVE_DUNE_DETAILED_DISCRETIZATIONS

// dune-stuff
#include <dune/stuff/grid/boundaryinfo.hh>
#include <dune/stuff/grid/intersection.hh>

namespace Dune {

namespace Stuff {

namespace DiscreteFunction {

namespace Projection {

namespace Dirichlet {

#ifdef HAVE_DUNE_DETAILED_DISCRETIZATIONS
template< class BoundaryInfoType,
          class FunctionType,
          class DiscreteFunctionSpaceImp, class VectorBackendImp >
void project(const BoundaryInfoType& boundaryInfo,
             const FunctionType& function,
             Dune::Detailed::Discretizations::DiscreteFunction::Default< DiscreteFunctionSpaceImp, VectorBackendImp >& discreteFunction)
{
  // some types
  typedef Dune::Detailed::Discretizations::DiscreteFunction::Default< DiscreteFunctionSpaceImp, VectorBackendImp > DiscreteFunctionType;
  typedef typename DiscreteFunctionType::DiscreteFunctionSpaceType DiscreteFunctionSpaceType;
  typedef typename DiscreteFunctionSpaceType::GridPartType GridPartType;
  typedef typename GridPartType::template Codim< 0 >::IteratorType EntityIteratorType;
  typedef typename GridPartType::template Codim< 0 >::EntityType EntityType;
  typedef typename EntityType::Geometry GeometryType;
  typedef typename GridPartType::IntersectionIteratorType IntersectionIteratorType;
  typedef typename IntersectionIteratorType::Intersection IntersectionType;
  typedef typename DiscreteFunctionSpaceType::BaseFunctionSetType BaseFunctionSetType;
  typedef typename DiscreteFunctionType::LocalFunctionType LocalFunctionType;
  typedef typename DiscreteFunctionSpaceType::DomainType DomainType;
  typedef typename DiscreteFunctionSpaceType::RangeType RangeType;
  // preparations
  RangeType tmpEvaluation(0.0);
  const DiscreteFunctionSpaceType& space = discreteFunction.space();
  const GridPartType& gridPart = space.gridPart();
  discreteFunction.clear();
  // walk the grid
  for (EntityIteratorType entityIt = gridPart.template begin< 0 >();
       entityIt != gridPart.template end< 0 >();
       ++entityIt)
  {
    // only consider entities with boundary intersection
    const EntityType& entity = *entityIt;
    const GeometryType& geometry = entity.geometry();
    if(entity.hasBoundaryIntersections()) {
      // local function of destination
      LocalFunctionType localFunction = discreteFunction.localFunction(entity);
      // get the Lagrange point set
      typedef typename DiscreteFunctionSpaceType::MapperType::LagrangePointSetType LagrangePointSetType;
      const LagrangePointSetType lagrangePointSet = space.map().lagrangePointSet(entity);
      // get the lagrange points' coordinates
      typedef typename LagrangePointSetType::CoordinateType LagrangePointCoordinateType;
      std::vector< LagrangePointCoordinateType > lagrangePointsGlobal(lagrangePointSet.nop(),
                                                                      LagrangePointCoordinateType(0.0));
      for (unsigned int i = 0; i < lagrangePointSet.nop(); ++i)
        lagrangePointsGlobal[i] = geometry.global(lagrangePointSet.point(i));
      // walk all intersections
      for (IntersectionIteratorType intersectionIt = gridPart.ibegin(entity);
           intersectionIt != gridPart.iend( entity );
           ++intersectionIt) {
        const IntersectionType& intersection = *intersectionIt;
        // only consider dirichlet boundary intersection
        if (boundaryInfo.dirichlet(intersection)) {
          // loop over all lagrange points
          for (unsigned int i = 0; i < lagrangePointSet.nop(); ++i )
          {
            // if dof lies on the boundary intersection
            if (Dune::Stuff::Grid::intersectionContains(intersection, lagrangePointsGlobal[i])) {
              // evaluate the function
              function.evaluate(lagrangePointsGlobal[i], tmpEvaluation);
              // set the corresponding local dof
              localFunction[i] = tmpEvaluation;
            } // if dof lies on the boundary intersection
          } // loop over all lagrange points
        } // only consider dirichlet boundary intersection
      } // walk all intersections
    } // only consider entities with boundary intersection
  } // walk the grid
} // static void project()
#endif // HAVE_DUNE_DETAILED_DISCRETIZATIONS

} // namespace Dirichlet

} // namespace Projection

} // namespace DiscreteFunction

} // namespace Stuff

} // namespace Dune

#endif // DUNE_STUFF_DISCRETEFUNCTION_PROJECTION_DIRICHLET_HH

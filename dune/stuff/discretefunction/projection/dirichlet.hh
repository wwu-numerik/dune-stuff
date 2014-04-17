// This file is part of the dune-stuff project:
//   http://users.dune-project.org/projects/dune-stuff
// Copyright holders: Rene Milk, Felix Schindler
// License: BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)

#ifndef DUNE_STUFF_DISCRETEFUNCTION_PROJECTION_DIRICHLET_HH
#define DUNE_STUFF_DISCRETEFUNCTION_PROJECTION_DIRICHLET_HH

#warning "This header is deprecated and will be removed in the future!"
#warning "Use #include <dune/gdt/operator/projections.hh> of the dune-gdt module instead!"

#include <vector>

#include <dune/stuff/grid/boundaryinfo.hh>
#include <dune/stuff/grid/intersection.hh>
#include <dune/stuff/functions/interfaces.hh>
#include <dune/stuff/common/color.hh>

#if HAVE_DUNE_GDT
  #include <dune/gdt/space/continuouslagrange/fem.hh>
  #include <dune/gdt/discretefunction/default.hh>
#endif // HAVE_DUNE_GDT

namespace Dune {
namespace Stuff {
namespace DiscreteFunction {


#if HAVE_DUNE_GDT
template< class GridPartType, int polOrder, class RangeFieldType, class VectorImp >
void DUNE_DEPRECATED_MSG("This function is deprecated! Use Dune::GDT::ProjectionOperator::Dirichlet instead!") project(const Dune::Stuff::GridboundaryInterface< typename GridPartType::IntersectionType >& boundaryInfo,
             const Dune::Stuff::FunctionInterface< typename GridPartType::ctype, GridPartType::dimension, RangeFieldType, 1, 1 >& source,
             Dune::GDT::DiscreteFunctionDefault<
                Dune::GDT::ContinuousLagrangeSpace::FemWrapper< GridPartType, polOrder, RangeFieldType, 1, 1 >,
                VectorImp
             >& target)
{
  typedef Dune::GDT::ContinuousLagrangeSpace::FemWrapper< GridPartType, polOrder, RangeFieldType, 1, 1 > SpaceType;
  // clear target function
  target.vector()->backend() *= RangeFieldType(0);
  // walk the grid
  const GridPartType& gridPart = target.space().gridPart();
  const auto entityEndIt = gridPart.template end< 0 >();
  for (auto entityIt = gridPart.template begin< 0 >(); entityIt != entityEndIt; ++entityIt) {
    const auto& entity = *entityIt;
    const auto& geometry = entity.geometry();
    // only consider entities with boundary intersections
    if(entity.hasBoundaryIntersections()) {
      const auto sourceLocalFunction = source.localFunction(entity);
      auto targetLocalFunction = target.localFunction(entity);
      auto targetLocalDofVector = targetLocalFunction.vector();
      const auto lagrangePointSet = target.space().backend().lagrangePointSet(entity);
      // get the lagrange points' coordinates
      typedef typename SpaceType::BackendType::LagrangePointSetType::CoordinateType LagrangePointCoordinateType;
      std::vector< LagrangePointCoordinateType > lagrangePoints(lagrangePointSet.nop(),
                                                                LagrangePointCoordinateType(0));
      for (size_t ii = 0; ii < lagrangePointSet.nop(); ++ii)
        lagrangePoints[ii] = geometry.global(lagrangePointSet.point(ii));
      // walk the intersections
      const auto intersectionEndIt = gridPart.iend(entity);
      for (auto intersectionIt = gridPart.ibegin(entity); intersectionIt != intersectionEndIt; ++intersectionIt) {
        const auto& intersection = *intersectionIt;
        // only consider dirichlet boundary intersection
        if (boundaryInfo.dirichlet(intersection)) {
          // loop over all lagrange points
          for (size_t ii = 0; ii < lagrangePointSet.nop(); ++ii) {
            // if dof lies on the boundary intersection
            if (Dune::Stuff::Grid::intersectionContains(intersection, lagrangePoints[ii])) {
              // set the corresponding target dof
              targetLocalDofVector.set(ii, sourceLocalFunction.evaluate(lagrangePointSet.point(ii)));
            } // if dof lies on the boundary intersection
          } // loop over all lagrange points
        } // only consider dirichlet boundary intersection
      } // walk the intersections
    } // only consider entities with boundary intersection
  } // walk the grid
} // static void project(...)
#endif // HAVE_DUNE_GDT


} // namespace DiscreteFunction
} // namespace Stuff
} // namespace Dune

#endif // DUNE_STUFF_DISCRETEFUNCTION_PROJECTION_DIRICHLET_HH

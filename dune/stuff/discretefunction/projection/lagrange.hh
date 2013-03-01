#ifndef DUNE_STUFF_DISCRETEFUNCTION_PROJECTION_LAGRANGE_HH
#define DUNE_STUFF_DISCRETEFUNCTION_PROJECTION_LAGRANGE_HH

#ifdef HAVE_CMAKE_CONFIG
  #include "cmake_config.h"
#else
  #include "config.h"
#endif // ifdef HAVE_CMAKE_CONFIG

#include <vector>

#include <dune/common/shared_ptr.hh>
#include <dune/common/static_assert.hh>

#if HAVE_DUNE_DETAILED_DISCRETIZATIONS
  #include <dune/detailed/discretizations/discretefunction/default.hh>
#endif // HAVE_DUNE_DETAILED_DISCRETIZATIONS

namespace Dune {
namespace Stuff {
namespace DiscreteFunction {
namespace Projection {
namespace Lagrange {

namespace DD = Dune::Detailed::Discretizations;

#if HAVE_DUNE_DETAILED_DISCRETIZATIONS
template< class SourceGridViewType,
          class TargetDiscreteFunctionSpaceType,
          class TargetVectorType >
void project(const Dune::VTKFunction< SourceGridViewType >& source,
             DD::DiscreteFunction::Default< TargetDiscreteFunctionSpaceType, TargetVectorType >& target)
{
  typedef DD::DiscreteFunction::Default< TargetDiscreteFunctionSpaceType, TargetVectorType > TargetFunctionType;
  dune_static_assert((TargetFunctionType::dimRange == 1), "Nope!");
  const auto& targetSpace = target.space();
  const auto& targetGridPart = targetSpace.gridPart();
  // walk the grid
  for (auto entityIt = targetGridPart.template begin< 0 >(); entityIt != targetGridPart.template end< 0 >(); ++entityIt)
  {
    const auto& entity = *entityIt;
    const auto& geometry = entity.geometry();
    auto localTargetFunction = target.localFunction(entity);
    // get the Lagrange point set
    const auto lagrangePointSet = targetSpace.map().lagrangePointSet(entity);
    for (size_t ii = 0; ii < lagrangePointSet.nop(); ++ii) {
      const auto lagrangePoint = geometry.global(lagrangePointSet.point(ii));
      localTargetFunction.set(ii, source.evaluate(0, entity, lagrangePoint));
    }
  } // walk the grid
} // static void project()
#endif // HAVE_DUNE_DETAILED_DISCRETIZATIONS

} // namespace Lagrange
} // namespace Projection
} // namespace DiscreteFunction
} // namespace Stuff
} // namespace Dune

#endif // DUNE_STUFF_DISCRETEFUNCTION_PROJECTION_LAGRANGE_HH

#ifndef DUNE_HELPER_TOOLS_DISCRETEFUNCTIONSPACE_PROJECTION_DIRICHLET_HH
#define DUNE_HELPER_TOOLS_DISCRETEFUNCTIONSPACE_PROJECTION_DIRICHLET_HH

// std includes
#include <vector>

// dune-stuff
#include <dune/stuff/grid/intersection.hh>

namespace Dune {
namespace Stuff {
namespace Projection {

class Dirichlet
{
public:

  template< class FunctionType, class DiscreteFunctionType >
  static void project(  const FunctionType& function,
                        DiscreteFunctionType& discreteFunction,
                        const int boundaryId = -1 )
  {
    typedef typename DiscreteFunctionType::DiscreteFunctionSpaceType
      DiscreteFunctionSpaceType;

    typedef typename DiscreteFunctionType::LocalFunctionType
      LocalFunctionType;

    typedef typename DiscreteFunctionSpaceType::GridPartType
      GridPartType;

    typedef typename DiscreteFunctionSpaceType::IteratorType
      EntityIteratorType;

    typedef typename GridPartType::IntersectionIteratorType
      IntersectionIteratorType;

    typedef typename IntersectionIteratorType::Intersection
      IntersectionType;

    typedef typename DiscreteFunctionSpaceType::BaseFunctionSetType
      BaseFunctionSetType;

    typedef typename DiscreteFunctionSpaceType::DomainType
      DomainType;

    typedef typename DiscreteFunctionSpaceType::RangeType
      RangeType;

    typedef typename DiscreteFunctionSpaceType::EntityType
      EntityType;

    typedef typename EntityType::Geometry
      GeometryType;

    RangeType tmpEvaluation(0.0);

    const DiscreteFunctionSpaceType& space = discreteFunction.space();

    // clear destination
    discreteFunction.clear();

    // do gridwalk
    const EntityIteratorType lastEntity = space.end();
    for( EntityIteratorType entityIterator = space.begin(); entityIterator != lastEntity ; ++entityIterator)
    {
      // entity
      const EntityType& entity = *entityIterator;

      // if this entity is a boundary entity
      if( entity.hasBoundaryIntersections() )
      {

        // geometry
        const GeometryType& entityGeometry = entity.geometry();

        // local function of destination
        LocalFunctionType localFunction = discreteFunction.localFunction( entity );

        // get the Lagrange point set
        typedef typename DiscreteFunctionSpaceType::MapperType::LagrangePointSetType
          LagrangePointSetType;

        const LagrangePointSetType lagrangePointSet = space.map().lagrangePointSet( entity );

        // get the lagrange points' coordinates
        typedef typename LagrangePointSetType::CoordinateType
          LagrangePointCoordinateType;

        std::vector< LagrangePointCoordinateType > lagrangePointsGlobal(  lagrangePointSet.nop(),
                                                                          LagrangePointCoordinateType( 0.0 ) );

        for( unsigned int i = 0; i < lagrangePointSet.nop(); ++i )
        {
          lagrangePointsGlobal[i] = entityGeometry.global( lagrangePointSet.point( i ) );
        }

        // do loop over all intersections
        const IntersectionIteratorType lastIntersection = space.gridPart().iend( entity );
        for(  IntersectionIteratorType intIt = space.gridPart().ibegin( entity );
              intIt != lastIntersection;
              ++intIt )
        {
          const IntersectionType& intersection = *intIt;

          // if this is a dirichlet boundary intersection
          if( !intersection.neighbor() && intersection.boundary()
              && ( boundaryId == -1 || intersection.boundaryId() ==boundaryId ) )
          {
            // do loop over all lagrange points
            for( unsigned int i = 0; i < lagrangePointSet.nop(); ++i )
            {
              // if dof lies on the boundary intersection
              if( Dune::HelperTools::Grid::Intersection::contains( intersection, lagrangePointsGlobal[i] ) )
              {
                // evaluate the boundary function
                function.evaluate( lagrangePointsGlobal[i], tmpEvaluation );
                // set the corresponding local dof
                localFunction[i] = tmpEvaluation;

              } // end if dof lies on the boundary intersection

            } // done loop over all lagrange points

          } // end if this is a boundary intersection

        } // done loop over all intersections

      } // end if this entity is a boundary entity

    } // done gridwalk

  } // end function project

private:

  class LocalBoundaryFunction
  {

  }; // end class LocalBoundaryFunction

}; // end class Dirichlet

} // end namespace Projection
} // end namespace Stuff
} // end namespace Dune

#endif // DUNE_HELPER_TOOLS_DISCRETEFUNCTIONSPACE_PROJECTION_DIRICHLET_HH

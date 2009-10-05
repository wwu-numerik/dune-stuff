#ifndef STUFF_GRID_HH_INCLUDED
#define STUFF_GRID_HH_INCLUDED

#include <dune/common/fvector.hh>

namespace Stuff {

/**
 *  \brief  calculates length of given intersection in world coordinates
 *  \tparam IntersectionIteratorType
 *          IntersectionIteratorType
 *  \param[in]  intIt
 *          intersection
 *  \return length of intersection
 **/
template < class IntersectionIteratorType >
double getLenghtOfIntersection( const IntersectionIteratorType& intIt )
{
    typedef typename IntersectionIteratorType::Geometry
        IntersectionGeometryType;
    const IntersectionGeometryType& intersectionGeoemtry = intIt.intersectionGlobal();
    return intersectionGeoemtry.volume();
//    assert( intersectionGeoemtry.corners() == 2 );
//    typedef typename IntersectionIteratorType::ctype
//        ctype;
//    const int dimworld = IntersectionIteratorType::dimensionworld;
//    typedef Dune::FieldVector< ctype, dimworld >
//        DomainType;
//    const DomainType cornerOne = intersectionGeoemtry[0];
//    const DomainType cornerTwo = intersectionGeoemtry[1];
//    const DomainType difference = cornerOne - cornerTwo;
//    return difference.two_norm();
}

template < class GridPartType, class DiscreteFunctionSpaceType, class OutStream >
void getGridInformation( GridPartType& gridPart, DiscreteFunctionSpaceType& space, OutStream& out )
{
    int numberOfEntities( 0 );
    int numberOfIntersections( 0 );
    int numberOfInnerIntersections( 0 );
    int numberOfBoundaryIntersections( 0 );
    double maxGridWidth( 0.0 );

    typedef typename GridPartType::GridType
        GridType;

    typedef typename GridType::template Codim< 0 >::Entity
        EntityType;

    typedef typename GridPartType::template Codim< 0 >::IteratorType
        EntityIteratorType;

    typedef typename GridPartType::IntersectionIteratorType
        IntersectionIteratorType;

    EntityIteratorType entityItEndLog = space.end();
    for (   EntityIteratorType entityItLog = space.begin();
            entityItLog != entityItEndLog;
            ++entityItLog ) {
        const EntityType& entity = *entityItLog;
        // count entities
        ++numberOfEntities;
        // walk the intersections
        IntersectionIteratorType intItEnd = gridPart.iend( entity );
        for (   IntersectionIteratorType intIt = gridPart.ibegin( entity );
                intIt != intItEnd;
                ++intIt ) {
            // count intersections
            ++numberOfIntersections;
            maxGridWidth = Stuff::getLenghtOfIntersection( intIt ) > maxGridWidth ? Stuff::getLenghtOfIntersection( intIt ) : maxGridWidth;
            // if we are inside the grid
            if ( intIt.neighbor() && !intIt.boundary() ) {
                // count inner intersections
                ++numberOfInnerIntersections;
            }
            // if we are on the boundary of the grid
            if ( !intIt.neighbor() && intIt.boundary() ) {
                // count boundary intersections
                ++numberOfBoundaryIntersections;
            }
        }
    }
    out << "found " << numberOfEntities << " entities," << std::endl;
    out << "found " << numberOfIntersections << " intersections," << std::endl;
    out << "      " << numberOfInnerIntersections << " intersections inside and" << std::endl;
    out << "      " << numberOfBoundaryIntersections << " intersections on the boundary." << std::endl;
    out << "      maxGridWidth is " << maxGridWidth << std::endl;
}



template < class Space, int codim = 0 >
class GridWalk {
    private:
        typedef typename Space::GridPartType
            GridPart;
        typedef typename GridPart::template Codim< codim >::IteratorType
            EntityIteratorType;
        typedef typename GridPart::IntersectionIteratorType
            IntersectionIteratorType;
        typedef typename IntersectionIteratorType::EntityPointer
            EntityPointer;

    public:
        GridWalk ( Space& gp )
            : space_(gp),
            gridPart_( gp.gridPart() )
        {
            EntityIteratorType entityItEndLog = space_.end();
            unsigned int en_idx = 0;
            for (   EntityIteratorType it = space_.begin();
                    it != entityItEndLog;
                    ++it,++en_idx )
            {
                entityIdxMap_.push_back( it );
            }
        }
        template < class Functor >
        void operator () ( Functor& f )
        {
            f.preWalk();
            EntityIteratorType entityItEndLog = space_.end();
            for (   EntityIteratorType it = space_.begin();
                    it != entityItEndLog;
                    ++it )
            {
                const int ent_idx = getIdx( entityIdxMap_, it );
                f( *it, *it, ent_idx, ent_idx);
                IntersectionIteratorType intItEnd = gridPart_.iend( *it );
                for (   IntersectionIteratorType intIt = gridPart_.ibegin( *it );
                        intIt != intItEnd;
                        ++intIt ) {
                    if ( !intIt.boundary() ) {
                        const int neigh_idx = getIdx( entityIdxMap_, intIt.outside() );
                        f( *it, *intIt.outside(), ent_idx, neigh_idx);
                    }
		}
            }
            f.postWalk();
        }
    private:
        Space& space_;
        GridPart& gridPart_;
        typedef std::vector< EntityPointer >
            EntityIdxMap;
        EntityIdxMap entityIdxMap_;
};

template < class GeometryType >
Dune::FieldVector< typename GeometryType::ctype, GeometryType::mydimension >  getBarycenterLocal( const GeometryType& geometry )
{
    assert( geometry.corners() > 0 );
    Dune::FieldVector< typename GeometryType::ctype, GeometryType::mydimension  > center;
    for( int i = 0; i < geometry.corners(); ++i ) {
        center += geometry.local( geometry[i] );
    }
    center /= geometry.corners();
    return center;
}

template < class GeometryType >
Dune::FieldVector< typename GeometryType::ctype, GeometryType::coorddimension >  getBarycenterGlobal( const GeometryType& geometry )
{
    assert( geometry.corners() > 0 );
    Dune::FieldVector< typename GeometryType::ctype, GeometryType::coorddimension > center;
    for( int i = 0; i < geometry.corners(); ++i ) {
        center += geometry[i] ;
    }
    center /= geometry.corners();
    return center;
}

}//end namespace

#endif

#ifndef STUFF_GRID_HH_INCLUDED
#define STUFF_GRID_HH_INCLUDED

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
    assert( intersectionGeoemtry.corners() == 2 );
    typedef typename IntersectionIteratorType::ctype
        ctype;
    const int dimworld = IntersectionIteratorType::dimensionworld;
    typedef Dune::FieldVector< ctype, dimworld >
        DomainType;
    const DomainType cornerOne = intersectionGeoemtry[0];
    const DomainType cornerTwo = intersectionGeoemtry[1];
    const DomainType difference = cornerOne - cornerTwo;
    return difference.two_norm();
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
                    const int neigh_idx = getIdx( entityIdxMap_, intIt.outside() );
                    if ( intIt.neighbor() ) {
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

}//end namespace

#endif

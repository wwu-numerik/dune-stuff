#ifndef STUFF_GRID_HH_INCLUDED
#define STUFF_GRID_HH_INCLUDED

#include "math.hh"
#include "misc.hh"
#include <dune/common/fvector.hh>
#include <vector>
#include <boost/format.hpp>

namespace Stuff {

/**
 *  \brief  calculates length of given intersection in world coordinates
 *  \tparam IntersectionType
 *          IntersectionType
 *  \param[in]  intersection
 *          intersection
 *  \return length of intersection
 **/
template < class IntersectionType >
double getLenghtOfIntersection( const IntersectionType& intersection )
{
	typedef typename IntersectionType::Geometry
        IntersectionGeometryType;
	const IntersectionGeometryType& intersectionGeoemtry = intersection.intersectionGlobal();
    return intersectionGeoemtry.volume();
}

/** \brief grid statistic output to given stream
  \todo not require a space to be passed
  */
template < class GridPartType, class DiscreteFunctionSpaceType, class OutStream >
void printGridInformation( GridPartType& gridPart, DiscreteFunctionSpaceType& space, OutStream& out )
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
            maxGridWidth = std::max( Stuff::getLenghtOfIntersection( intIt ), maxGridWidth );
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

//! Base class for Gridwalk Functors that don't want to reimplement pre/postWalk
struct GridwalkFunctorDefault {
	void preWalk() const{}
	void postWalk()const{}
};

/** \brief lets you apply a Functor to each entity
  \todo not require a space to be passed
  \todo allow stacking of operators to save gridwalks
  \todo threadsafe maps (haha:P)
  */
template < class GridView, int codim = 0 >
class GridWalk {
    private:
		typedef typename GridView::template Codim<0>::Iterator
			ElementIterator;
		typedef typename GridView::IntersectionIterator
            IntersectionIteratorType;
        typedef typename IntersectionIteratorType::EntityPointer
            EntityPointer;

    public:
		GridWalk ( const GridView& gp )
			: gridView_( gp )
        {
            unsigned int en_idx = 0;
			for (ElementIterator it = gridView_.template begin<0>();
				 it!=gridView_.template end<0>(); ++it,++en_idx)
            {
				entityIdxMap_.push_back( *it );
            }
        }

		template < class Functor >
		void operator () ( Functor& f ) const
		{
			f.preWalk();
			for (ElementIterator it = gridView_.template begin<0>();
				 it!=gridView_.template end<0>(); ++it)
			{
				const int ent_idx = getIdx( entityIdxMap_, *it );
				f( *it, *it, ent_idx, ent_idx);
				IntersectionIteratorType intItEnd = gridView_.iend( *it );
				for (   IntersectionIteratorType intIt = gridView_.ibegin( *it );
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

		template < class Functor >
		void walkCodim0( Functor& f ) const
		{
			for (ElementIterator it = gridView_.template begin<0>();
				 it!=gridView_.template end<0>(); ++it)
			{
				const int ent_idx = getIdx( entityIdxMap_, *it );
				f( *it, ent_idx );
			}
		}

    private:
		const GridView& gridView_;
        typedef std::vector< EntityPointer >
            EntityIdxMap;
        EntityIdxMap entityIdxMap_;
};

//! gets barycenter of given geometry in local coordinates
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

//! gets barycenter of given geometry in global coordinates
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

template < class GridType >
struct GridDimensions {
	typedef MinMaxAvg<typename GridType::ctype>
		MinMaxAvgType;
	typedef Dune::array< MinMaxAvgType, GridType::dimensionworld >
		CoordLimitsType;
	CoordLimitsType coord_limits;
	MinMaxAvgType entity_volume;

	struct GridDimensionsFunctor {
		CoordLimitsType& coord_limits_;
		MinMaxAvgType& entity_volume_;

		GridDimensionsFunctor( CoordLimitsType& c, MinMaxAvgType& e ):coord_limits_(c),entity_volume_(e){}

		template <class Entity>
		void operator() ( const Entity& ent, const int /*ent_idx*/ )
		{
			typedef typename Entity::Geometry
				EntityGeometryType;
			typedef Dune::FieldVector< typename EntityGeometryType::ctype, EntityGeometryType::coorddimension>
				DomainType;
			const typename Entity::Geometry& geo = ent.geometry();
			entity_volume_( geo.volume() );
			for ( int i = 0; i < geo.corners(); ++i )
			{
				const DomainType& corner( geo.corner( i ) );
				for ( size_t k = 0; k < GridType::dimensionworld; ++k )
					coord_limits_[k]( corner[k] );
			}
		}
	};

	double volumeRelation() const
	{ return entity_volume.min() != 0.0 ? entity_volume.max() / entity_volume.min() : -1; }

	GridDimensions( const GridType& grid )
	{
		typedef typename GridType::LeafGridView
			View;
		const View& view = grid.leafView();
		GridDimensionsFunctor f( coord_limits, entity_volume );
		GridWalk<View>( view ).walkCodim0( f );
	}
};

template <class Stream, class T>
inline Stream& operator<< (Stream& s, const GridDimensions<T>& d )
{
	for ( size_t k = 0; k < T::dimensionworld; ++k )
	{
		const typename GridDimensions<T>::MinMaxAvgType& mma = d.coord_limits[k];
		s << boost::format( "x%d\tmin: %e\tavg: %e\tmax: %e\n" )
			 % k
			 % mma.min()
			 % mma.average()
			 % mma.max();
	}
	s << boost::format("Entity vol min: %e\tavg: %e\tmax: %e\tQout: %e")
		 % d.entity_volume.min()
		 % d.entity_volume.average()
		 % d.entity_volume.max()
		 % d.volumeRelation();
	s << std::endl;
	return s;
}

template < class GridType >
struct MaximumEntityVolumeRefineFunctor {
	MaximumEntityVolumeRefineFunctor ( GridType& grid, double volume, double factor )
		:threshold_volume_( volume * factor ),
		grid_(grid)
	{}

	template <class Entity>
	void operator() ( const Entity& ent, const int /*ent_idx*/ )
	{
		const double volume = ent.geometry().volume();
		if ( volume > threshold_volume_ )
			grid_.mark( 1, ent );
	}

	const double threshold_volume_;
	GridType& grid_;
};

//! refine entities until all have volume < size_factor * unrefined_minimum_volume
template < class GridType >
void EnforceMaximumEntityVolume( GridType& grid, const double size_factor )
{
	const GridDimensions<GridType> unrefined_dimensions( grid );
	const double unrefined_min_volume = unrefined_dimensions.entity_volume.min();
	typedef typename GridType::LeafGridView
		View;
	View view = grid.leafView();
	MaximumEntityVolumeRefineFunctor<GridType> f( grid, unrefined_min_volume, size_factor );
	while ( true )
	{
		grid.preAdapt();
		GridWalk<View>( view ).walkCodim0( f );
		if ( !grid.adapt() )
			break;
		grid.postAdapt();
		std::cout << Stuff::GridDimensions<GridType>( grid );
	}
}

}//end namespace

#endif

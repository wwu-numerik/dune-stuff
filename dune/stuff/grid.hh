#ifndef STUFF_GRID_HH_INCLUDED
#define STUFF_GRID_HH_INCLUDED

#include "math.hh"
#include "misc.hh"
#include <dune/common/static_assert.hh>
#include <dune/common/fvector.hh>
#include <dune/stuff/deprecated.hh>
#include <dune/stuff/math.hh>
#include <dune/grid/common/geometry.hh>
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
template< class IntersectionType >
double getLenghtOfIntersection(const IntersectionType& intersection) {
  typedef typename IntersectionType::Geometry
  IntersectionGeometryType;
  const IntersectionGeometryType& intersectionGeoemtry = intersection.geometry();
  return intersectionGeoemtry.volume();
}

/** \brief grid statistic output to given stream
   * \todo not require a space to be passed
   */
template< class GridPartType, class DiscreteFunctionSpaceType, class OutStream >
void printGridInformation(GridPartType& gridPart, DiscreteFunctionSpaceType& space, OutStream& out) {
  int numberOfEntities(0);
  int numberOfIntersections(0);
  int numberOfInnerIntersections(0);
  int numberOfBoundaryIntersections(0);
  double maxGridWidth(0.0);

  typedef typename GridPartType::GridType
  GridType;

  typedef typename GridType::template Codim< 0 >::Entity
  EntityType;

  typedef typename GridPartType::template Codim< 0 >::IteratorType
  EntityIteratorType;

  typedef typename GridPartType::IntersectionIteratorType
  IntersectionIteratorType;

  EntityIteratorType entityItEndLog = space.end();
  for (EntityIteratorType entityItLog = space.begin();
       entityItLog != entityItEndLog;
       ++entityItLog)
  {
    const EntityType& entity = *entityItLog;
    // count entities
    ++numberOfEntities;
    // walk the intersections
    IntersectionIteratorType intItEnd = gridPart.iend(entity);
    for (IntersectionIteratorType intIt = gridPart.ibegin(entity);
         intIt != intItEnd;
         ++intIt)
    {
      // count intersections
      ++numberOfIntersections;
      maxGridWidth = std::max(Stuff::getLenghtOfIntersection(intIt), maxGridWidth);
      // if we are inside the grid
      if ( intIt.neighbor() && !intIt.boundary() )
      {
        // count inner intersections
        ++numberOfInnerIntersections;
      }
      // if we are on the boundary of the grid
      if ( !intIt.neighbor() && intIt.boundary() )
      {
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
} // printGridInformation

// ! Base class for Gridwalk Functors that don't want to reimplement pre/postWalk
struct GridwalkFunctorDefault
{
  void preWalk() const {}
  void postWalk() const {}
};

/** \brief lets you apply a Functor to each entity
   * \todo allow stacking of operators to save gridwalks
   * \todo threadsafe maps (haha:P)
   */
template< class GridView, int codim = 0 >
class GridWalk
{
private:
  typedef typename GridView::template Codim< 0 >::Iterator
  ElementIterator;
  typedef typename GridView::IntersectionIterator
  IntersectionIteratorType;
  typedef typename IntersectionIteratorType::Intersection::EntityPointer
  EntityPointer;

public:
  GridWalk(const GridView& gp)
    : gridView_(gp)
  {}

  template< class Functor >
  void operator()(Functor& f) const {
    f.preWalk();
    for (ElementIterator it = gridView_.template begin< 0 >();
         it != gridView_.template end< 0 >(); ++it)
    {
      const int ent_idx = gridView_.indexSet().index(*it);
      f(*it, *it, ent_idx, ent_idx);
      IntersectionIteratorType intItEnd = gridView_.iend(*it);
      for (IntersectionIteratorType intIt = gridView_.ibegin(*it);
           intIt != intItEnd;
           ++intIt)
      {
        const auto& intersection = *intIt;
        if ( !intersection.boundary() )
        {
          const auto neighbour_ptr = intersection.outside();
          const int neigh_idx = gridView_.indexSet().index(*neighbour_ptr);
          f(*it, *neighbour_ptr, ent_idx, neigh_idx);
        }
      }
    }
    f.postWalk();
  } // ()

  template< class Functor >
  void walkCodim0(Functor& f) const {
    for (ElementIterator it = gridView_.template begin< 0 >();
         it != gridView_.template end< 0 >(); ++it)
    {
      const int ent_idx = gridView_.indexSet().index(*it);
      f(*it, ent_idx);
    }
  } // walkCodim0

private:
  const GridView& gridView_;
};

// ! gets barycenter of given geometry in local coordinates
template< class GeometryType >
DUNE_DEPRECATED_MSG("use geometry.local( geometry.center() ) instead") Dune::FieldVector< typename GeometryType::ctype,
                                                                                          GeometryType::mydimension >
getBarycenterLocal(const GeometryType& geometry) {
  return geometry.local( geometry.center() );
}

// ! gets barycenter of given geometry in global coordinates
template< class GeometryType >
DUNE_DEPRECATED_MSG("use geometry.center() directly") Dune::FieldVector< typename GeometryType::ctype,
                                                                         GeometryType::coorddimension >
getBarycenterGlobal(const GeometryType& geometry) {
  return geometry.center();
}

// ! Provide min/max coordinates for all space dimensions of a Grid (in the leafView)
template< class GridType >
struct GridDimensions
{
  // ! automatic running min/max
  typedef MinMaxAvg< typename GridType::ctype >
  MinMaxAvgType;
  typedef Dune::array< MinMaxAvgType, GridType::dimensionworld >
  CoordLimitsType;
  CoordLimitsType coord_limits;
  MinMaxAvgType entity_volume;

  // ! gridwalk functor that does the actual work for \ref GridDimensions
  class GridDimensionsFunctor
  {
    CoordLimitsType& coord_limits_;
    MinMaxAvgType& entity_volume_;

public:
    GridDimensionsFunctor(CoordLimitsType& c, MinMaxAvgType& e)
      : coord_limits_(c)
        , entity_volume_(e) {}

    template< class Entity >
    void operator()(const Entity& ent, const int /*ent_idx*/) {
      typedef typename Entity::Geometry
      EntityGeometryType;
      typedef Dune::FieldVector< typename EntityGeometryType::ctype, EntityGeometryType::coorddimension >
      DomainType;
      const typename Entity::Geometry& geo = ent.geometry();
      entity_volume_( geo.volume() );
      for (int i = 0; i < geo.corners(); ++i)
      {
        const DomainType& corner( geo.corner(i) );
        for (size_t k = 0; k < GridType::dimensionworld; ++k)
          coord_limits_[k](corner[k]);
      }
    } // ()
  };

  double volumeRelation() const
  { return entity_volume.min() != 0.0 ? entity_volume.max() / entity_volume.min() : -1; }

  GridDimensions(const GridType& grid) {
    typedef typename GridType::LeafGridView
    View;
    const View& view = grid.leafView();
    GridDimensionsFunctor f(coord_limits, entity_volume);
    GridWalk< View >(view).walkCodim0(f);
  }
};

template< class Stream, class T >
inline Stream& operator<<(Stream& s, const GridDimensions< T >& d) {
  for (size_t k = 0; k < T::dimensionworld; ++k)
  {
    const typename GridDimensions< T >::MinMaxAvgType& mma = d.coord_limits[k];
    s << boost::format("x%d\tmin: %e\tavg: %e\tmax: %e\n")
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
} // <<

// ! GridWalk functor that refines all entitites above given volume
template< class GridType >
struct MaximumEntityVolumeRefineFunctor
{
  MaximumEntityVolumeRefineFunctor(GridType& grid, double volume, double factor)
    : threshold_volume_(volume * factor)
      , grid_(grid)
  {}

  template< class Entity >
  void operator()(const Entity& ent, const int /*ent_idx*/) {
    const double volume = ent.geometry().volume();

    if (volume > threshold_volume_)
      grid_.mark(1, ent);
  }

  const double threshold_volume_;
  GridType& grid_;
};

// ! refine entities until all have volume < size_factor * unrefined_minimum_volume
template< class GridType >
void EnforceMaximumEntityVolume(GridType& grid, const double size_factor) {
  const GridDimensions< GridType > unrefined_dimensions(grid);
  const double unrefined_min_volume = unrefined_dimensions.entity_volume.min();
  typedef typename GridType::LeafGridView
  View;
  View view = grid.leafView();
  MaximumEntityVolumeRefineFunctor< GridType > f(grid, unrefined_min_volume, size_factor);
  while (true)
  {
    grid.preAdapt();
    GridWalk< View >(view).walkCodim0(f);
    if ( !grid.adapt() )
      break;
    grid.postAdapt();
    std::cout << Stuff::GridDimensions< GridType >(grid);
  }
} // EnforceMaximumEntityVolume

template< class GridImp, template< int, int, class > class EntityImp >
double geometryDiameter(const Dune::Entity< 0, 2, GridImp, EntityImp >& entity) {
  typedef Dune::Entity< 0, 2, GridImp, EntityImp >
  EntityType;
  typedef typename EntityType::LeafIntersectionIterator
  IntersectionIteratorType;
  IntersectionIteratorType end = entity.ileafend();
  double factor = 1.0;
  for (IntersectionIteratorType it = entity.ileafbegin(); it != end; ++it)
  {
    const typename IntersectionIteratorType::Intersection& intersection = *it;
    factor *= intersection.geometry().volume();
  }
  return factor / ( 2.0 * entity.geometry().volume() );
} // geometryDiameter

template< class GridImp, template< int, int, class > class EntityImp >
double geometryDiameter(const Dune::Entity< 0, 3, GridImp, EntityImp >& entity) {
  DUNE_THROW(Dune::Exception, "copypasta from 2D");
  typedef Dune::Entity< 0, 3, GridImp, EntityImp >
  EntityType;
  typedef typename EntityType::LeafIntersectionIterator
  IntersectionIteratorType;
  IntersectionIteratorType end = entity.ileafend();
  double factor = 1.0;
  for (IntersectionIteratorType it = entity.ileafbegin(); it != end; ++it)
  {
    const typename IntersectionIteratorType::Intersection& intersection = *it;
    factor *= intersection.geometry().volume();
  }
  return factor / ( 2.0 * entity.geometry().volume() );
} // geometryDiameter
} // end namespace

#endif // ifndef STUFF_GRID_HH_INCLUDED
/** Copyright (c) 2012, Felix Albrecht, Rene Milk
   * All rights reserved.
   *
   * Redistribution and use in source and binary forms, with or without
   * modification, are permitted provided that the following conditions are met:
   *
   * 1. Redistributions of source code must retain the above copyright notice, this
   *    list of conditions and the following disclaimer.
   * 2. Redistributions in binary form must reproduce the above copyright notice,
   *    this list of conditions and the following disclaimer in the documentation
   *    and/or other materials provided with the distribution.
   *
   * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
   * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
   * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
   * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
   * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
   * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
   * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
   * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
   * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
   * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
   *
   * The views and conclusions contained in the software and documentation are those
   * of the authors and should not be interpreted as representing official policies,
   * either expressed or implied, of the FreeBSD Project.
   **/

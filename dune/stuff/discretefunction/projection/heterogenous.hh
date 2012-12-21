#ifndef DUNE_STUFF_DISCRETEFUNCTION_PROJECTION_DIRICHLET_HH
#define DUNE_STUFF_DISCRETEFUNCTION_PROJECTION_DIRICHLET_HH

#ifdef HAVE_CMAKE_CONFIG
 #include "cmake_config.h"
#else
 #include "config.h"
#endif // ifdef HAVE_CMAKE_CONFIG

#include <vector>

#if HAVE_DUNE_GEOMETRY
#include <dune/geometry/referenceelements.hh>
#else
#include <dune/grid/common/genericreferenceelements.hh>
#endif

#include <dune/common/shared_ptr.hh>
#include <dune/common/fvector.hh>
#include <dune/grid/common/backuprestore.hh>
#include <dune/grid/common/grid.hh>
#include <dune/grid/common/gridview.hh>
#include <dune/grid/common/entity.hh>
#include <dune/stuff/common/ranges.hh>
#include <dune/stuff/aliases.hh>

//#ifdef HAVE_DUNE_FEM
#include <dune/fem/function/common/discretefunction.hh>
#include <dune/fem/quadrature/cachingquadrature.hh>
#include <dune/fem/space/dgspace/localdgmassmatrix.hh>
//#endif

#include <boost/range/iterator_range.hpp>

namespace Dune {
namespace Stuff {


template <class ViewTraits>
class NaiveSearchStrategy {

  typedef typename ViewTraits::template Codim<0>::Entity EntityType;
  typedef typename EntityType::Geometry::LocalCoordinate LocalCoordinateType;
  typedef typename EntityType::Geometry::GlobalCoordinate GlobalCoordinateType;

public:
  NaiveSearchStrategy(const Dune::GridView<ViewTraits>& gridview)
    : gridview_(gridview)
  {}

  template <template<int,int,class> class ForeignEntityImp, class ForeignGridImp>
  std::vector<typename EntityType::EntityPointer>
  operator() (const Dune::Entity<EntityType::codimension, EntityType::dimension, ForeignGridImp, ForeignEntityImp>& other) const
  {
    for(const auto& my_ent : range) {

    }
  }

private:
  const Dune::GridView<ViewTraits>& gridview_;
};

template <class ViewTraits>
class DefaultSearchStrategy {

  typedef typename ViewTraits::template Codim<0>::Entity EntityType;
  typedef typename EntityType::Geometry::LocalCoordinate LocalCoordinateType;
  typedef typename EntityType::Geometry::GlobalCoordinate GlobalCoordinateType;

  int countInside(const std::vector<GlobalCoordinateType>& corners,
                  const EntityType& entity) const {
    int ret = 0;
    const auto& geometry = entity.geometry();
    const auto& refElement
      = GenericReferenceElements< typename LocalCoordinateType::value_type, LocalCoordinateType::dimension >::general(geometry.type());
    for(const auto corner : corners) {
      const auto xlocal = geometry.local(corner);
      ret += refElement.checkInside(xlocal);
    }
    return ret;
  }

  const Dune::GridView<ViewTraits>& gridview_;

public:

  DefaultSearchStrategy(const Dune::GridView<ViewTraits>& gridview)
    : gridview_(gridview)
  {}

  template <template<int,int,class> class ForeignEntityImp, class ForeignGridImp>
  std::vector<typename EntityType::EntityPointer>
  operator() (const Dune::Entity<EntityType::codimension, EntityType::dimension, ForeignGridImp, ForeignEntityImp>& other) const
  {
    auto range = DSC::viewRange(gridview_.grid().levelView(0));
    return process(other, range);
  }

private:
  template <template<int,int,class> class ForeignEntityImp, class ForeignGridImp>
  static std::vector<GlobalCoordinateType>
  getCorners(const Dune::Entity<EntityType::codimension, EntityType::dimension, ForeignGridImp, ForeignEntityImp>& other) {
    std::vector<GlobalCoordinateType> ret;
    for(int i = 0;i < other.geometry().corners(); ++i) {
      auto c = other.geometry().corner(i);
      ret.push_back(c);
    }
    return ret;
  }

  template <template<int,int,class> class ForeignEntityImp, class ForeignGridImp, class RangeType>
  std::vector<typename EntityType::EntityPointer>
  process(const Dune::Entity<EntityType::codimension, EntityType::dimension, ForeignGridImp, ForeignEntityImp>& other,
          const RangeType& range) const
  {
    std::vector<typename EntityType::EntityPointer> ret;
    auto corners = getCorners(other);
    for(const auto& my_ent : range) {
      const int my_level = my_ent.level();
      const int inside = countInside(corners, my_ent);
      if (inside > 0) {
        //if I cannot descend further add this entity even if it's not my view
        if(gridview_.grid().maxLevel() <= my_level || gridview_.contains(my_ent)) {
          ret.emplace_back(my_ent);
        }
        else {
          const auto h_end = my_ent.hend(my_level+1);
          const auto h_begin = my_ent.hbegin(my_level+1);
          auto h_range = boost::make_iterator_range(h_begin, h_end);
          const auto kids = process(other, h_range);
          ret.insert(ret.end(), kids.begin(), kids.end());
        }
      }
    }
    return ret;
  }
};

template <template <class> class SearchStrategy = DefaultSearchStrategy>
class HeterogenousProjection {

  template <class T, int I, class EntityType>
  int selectEntity(const Dune::FieldVector<T,I>& point, const std::vector<EntityType>& entities)
  {
    for(int i = 0; i < entities.size(); ++i) {
      const auto& entity = entities[i];
      if(entity.geometry().checkInside(point))
        return i;
    }
    return -1;
  }

//  template < class SourceDF, class TargetDF >
//  void project(const SourceDF& /*source*/, const TargetDF& /*target*/)
//  {
//  }

public:
  template < class SourceDFImp, class TargetDFImp >
  static void project(const Dune::DiscreteFunctionInterface<SourceDFImp>& source,
                      Dune::DiscreteFunctionInterface<TargetDFImp>& target,
                      const int polOrd = -1)
{
  typedef typename SourceDFImp::GridType::LeafGridView SourceLeafGridView;
  SearchStrategy<typename SourceLeafGridView::Traits> search(source.gridPart().grid().leafView());

  typedef typename TargetDFImp::DiscreteFunctionSpaceType DiscreteFunctionSpaceType;
  typedef typename TargetDFImp::LocalFunctionType LocalFuncType;
  typedef typename DiscreteFunctionSpaceType::Traits::GridPartType GridPartType;
  typedef typename DiscreteFunctionSpaceType::Traits::IteratorType Iterator;
  typedef typename DiscreteFunctionSpaceType::BaseFunctionSetType BaseFunctionSetType ;
  typedef typename TargetDFImp::GridType GridType;


  const auto& space =  target.space();

  // type of quadrature
  typedef Dune::CachingQuadrature<GridPartType,0> QuadratureType;
  // type of local mass matrix
  typedef Dune::LocalMassMatrix< DiscreteFunctionSpaceType, QuadratureType > LocalMassMatrixType;

  const int quadOrd = (polOrd == -1) ? (2 * space.order()) : polOrd;

  // create local mass matrix object
  LocalMassMatrixType massMatrix(space, quadOrd);

  // clear destination
  target.clear();

  // extract types from grid part
  typedef typename GridPartType::template Codim<0>::EntityType  EntityType ;

  const Iterator endit = space.end();
  for(Iterator it = space.begin(); it != endit ; ++it)
  {
    // get entity
    const EntityType& en = *it;
    const auto possible_entities = search(en);
    // get geometry
    const auto& geo = en.geometry();

    // get quadrature
    QuadratureType quad(en, quadOrd);

    // get local function of destination
    LocalFuncType lf = target.localFunction(en);

    const int quadNop = quad.nop();

    typename DiscreteFunctionSpaceType :: RangeType value ;

    for(int qP = 0; qP < quadNop ; ++qP)
    {
      const auto point = quad.point(qP);
      const double intel =
           quad.weight(qP) * geo.integrationElement(point);

      const auto local_point = quad[ qP ];
      const auto global_point = geo.global(point);
      typedef typename EntityType::Geometry::LocalCoordinate LocalCoordinateType;
      // evaluate source function
      for(const auto& ent_p : possible_entities) {
        const auto& p_geometry = ent_p->geometry();
        const auto p_local = p_geometry.local(global_point);
        const auto& p_refElement
          = GenericReferenceElements< typename LocalCoordinateType::value_type, LocalCoordinateType::dimension >::general(p_geometry.type());
        if (p_refElement.checkInside(p_local)) {
          const auto p_local_function = source.localFunction(*ent_p);
          p_local_function.evaluate(p_local, value );
          value /= geo.volume();
          break;
        }
      }

      // apply weight
      value *= intel;

      // add to local function
      lf.axpy( quad[ qP ], value );
    }
//    massMatrix.applyInverse( en, lf );
  }
}
};

} // namespace Stuff
} // namespace Dune

#endif // DUNE_STUFF_DISCRETEFUNCTION_PROJECTION_DIRICHLET_HH

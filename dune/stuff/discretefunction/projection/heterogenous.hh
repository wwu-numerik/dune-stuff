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
class StrategyBase {
public:
  typedef typename ViewTraits::template Codim<0>::Entity EntityType;
  typedef typename EntityType::Geometry::LocalCoordinate LocalCoordinateType;
  typedef typename EntityType::Geometry::GlobalCoordinate GlobalCoordinateType;
  typedef std::vector<typename EntityType::EntityPointer> EntityPointerVector;

protected:
  template <template<int,int,class> class ForeignEntityImp, class ForeignGridImp>
  static inline std::vector<GlobalCoordinateType>
  getCorners(const Dune::Entity<EntityType::codimension, EntityType::dimension, ForeignGridImp, ForeignEntityImp>& other)
  {
    std::vector<GlobalCoordinateType> ret;
    for(int i = 0;i < other.geometry().corners(); ++i) {
      ret.emplace_back(other.geometry().corner(i));
    }
    return ret;
  }

  static inline
  int countInside(const std::vector<GlobalCoordinateType>& corners,
                  const EntityType& entity) {
    int ret = 0;
    const auto& geometry = entity.geometry();
    const auto& refElement
      = GenericReferenceElements< typename LocalCoordinateType::value_type,
                                  LocalCoordinateType::dimension >::general(geometry.type());
    for(const auto corner : corners) {
      const auto xlocal = geometry.local(corner);
      ret += refElement.checkInside(xlocal);
    }
    return ret;
  }
};

template <class ViewTraits>
class NaiveSearchStrategy : public StrategyBase<ViewTraits> {
  typedef StrategyBase<ViewTraits> BaseType;
  typedef GenericReferenceElements< typename  BaseType::LocalCoordinateType::value_type,
                                              BaseType::LocalCoordinateType::dimension >
        RefElementType;

public:
  NaiveSearchStrategy(const Dune::GridView<ViewTraits>& gridview)
    : gridview_(gridview)
  {}

  template <template<int,int,class> class ForeignEntityImp, class ForeignGridImp>
  typename BaseType::EntityPointerVector
  operator() (const Dune::Entity<BaseType::EntityType::codimension, BaseType::EntityType::dimension, ForeignGridImp, ForeignEntityImp>& other) const
  {
    std::vector<typename BaseType::EntityType::EntityPointer> ret;
    for(const auto& entity : DSC::viewRange(gridview_)) {
      const auto& geometry = entity.geometry();
      const auto& refElement = RefElementType::general(geometry.type());
      for(const auto& corner : BaseType::getCorners(other)) {
        if(refElement.checkInside(geometry.local(corner)))
        {
          ret.emplace_back(entity);
          break;
        }
      }
    }
    return ret;
  }

private:
  const Dune::GridView<ViewTraits>& gridview_;
};

template <class ViewTraits>
class DefaultSearchStrategy : public StrategyBase<ViewTraits> {
  typedef StrategyBase<ViewTraits> BaseType;

  const Dune::GridView<ViewTraits>& gridview_;
  const int start_level_;

public:
  DefaultSearchStrategy(const Dune::GridView<ViewTraits>& gridview)
    : gridview_(gridview)
    , start_level_(0)
  {}

  template <template<int,int,class> class ForeignEntityImp, class ForeignGridImp>
  typename BaseType::EntityPointerVector
  operator() (const Dune::Entity<BaseType::EntityType::codimension, BaseType::EntityType::dimension, ForeignGridImp, ForeignEntityImp>& other) const
  {
    auto level = std::min(gridview_.grid().maxLevel(), start_level_);
    auto range = DSC::viewRange(gridview_.grid().levelView(level));
    return process(other, range);
  }

private:

  template <template<int,int,class> class ForeignEntityImp, class ForeignGridImp, class RangeType>
  std::vector<typename BaseType::EntityType::EntityPointer>
  process(const Dune::Entity<BaseType::EntityType::codimension, BaseType::EntityType::dimension, ForeignGridImp, ForeignEntityImp>& other,
          const RangeType& range) const
  {
    std::vector<typename BaseType::EntityType::EntityPointer> ret;
    auto corners = BaseType::getCorners(other);
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
          const auto h_range = boost::make_iterator_range(h_begin, h_end);
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
  typedef typename TargetDFImp::DofType DofType;
  typedef typename TargetDFImp::GridType GridType;
  typedef Dune::CachingQuadrature<GridPartType,0> QuadratureType;
  typedef typename GridPartType::template Codim<0>::EntityType  EntityType ;
  static const int dimRange = DiscreteFunctionSpaceType::dimRange;
  typedef typename EntityType::Geometry::LocalCoordinate LocalCoordinateType;
  typedef GenericReferenceElements< typename LocalCoordinateType::value_type,
      LocalCoordinateType::dimension > RefElementType;

  const auto& space =  target.space();
  const int quadOrd = (polOrd == -1) ? (2 * space.order()) : polOrd;

  // set all DoFs to infinity
  const auto dend = target.dend();
  for( auto dit = target.dbegin(); dit != dend; ++dit )
    *dit = std::numeric_limits< DofType >::infinity();

  const Iterator endit = space.end();
  for(Iterator it = space.begin(); it != endit ; ++it)
  {
    // get entity
    const EntityType& en = *it;
    const auto possible_entities = search(en);
    // get geometry
    const auto& geo = en.geometry();

    // get quadrature
    const QuadratureType quad(en, quadOrd);

    // get local function of destination
    LocalFuncType target_lf = target.localFunction(en);

    const int quadNop = quad.nop();

    typename DiscreteFunctionSpaceType :: RangeType value ;
    int k = 0;
    for(int qP = 0; qP < quadNop ; ++qP)
    {
      if( target_lf[ k ] == std::numeric_limits< DofType >::infinity() )
      {
        const auto point = quad.point(qP);
        const auto global_point = geo.global(point);

        // evaluate source function
        bool evaluated = false;
        for(const auto& ent_p : possible_entities) {
          const auto& p_geometry = ent_p->geometry();
          const auto p_local = p_geometry.local(global_point);
          const auto& p_refElement = RefElementType::general(p_geometry.type());
          if (p_refElement.checkInside(p_local)) {
            const auto p_local_function = source.localFunction(*ent_p);
            p_local_function.evaluate(p_local, value );
            for( int i = 0; i < dimRange; ++i, ++k )
              target_lf[ k ] = value[ i ];
            evaluated = true;
            break;
          }
        }
        if(not evaluated)
          DUNE_THROW(InvalidStateException, "did not eval in quad point");
      }
      else
        k += dimRange;
    }
  }
}
};

} // namespace Stuff
} // namespace Dune

#endif // DUNE_STUFF_DISCRETEFUNCTION_PROJECTION_DIRICHLET_HH

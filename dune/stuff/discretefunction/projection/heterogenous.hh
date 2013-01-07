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
};

template <class ViewTraits>
class NaiveSearchStrategy : public StrategyBase<ViewTraits> {
  typedef StrategyBase<ViewTraits> BaseType;
  typedef GenericReferenceElements< typename  BaseType::LocalCoordinateType::value_type,
                                              BaseType::LocalCoordinateType::dimension >
        RefElementType;
  typedef typename Dune::GridView<ViewTraits>::template Codim< 0 >::Iterator IteratorType;


  inline bool check_add(const typename BaseType::EntityType& entity,
                        const typename BaseType::GlobalCoordinateType& point,
                        typename BaseType::EntityPointerVector& ret) const {
    const auto& geometry = entity.geometry();
    const auto& refElement = RefElementType::general(geometry.type());
    if(refElement.checkInside(geometry.local(point)))
    {
      ret.emplace_back(entity);
      return true;
    }
    return false;
  }
public:
  NaiveSearchStrategy(const Dune::GridView<ViewTraits>& gridview)
    : gridview_(gridview)
    , it_last_(gridview_.template begin< 0 >())
  {}

  template <class QuadpointContainerType>
  typename BaseType::EntityPointerVector operator() (const QuadpointContainerType& quad_points)
  {
    const auto max_size = quad_points.size();

    const IteratorType begin = gridview_.template begin< 0 >();
    const IteratorType end = gridview_.template end< 0 >();
    std::vector<typename BaseType::EntityType::EntityPointer> ret;
    for(const auto& point : quad_points)
    {
      IteratorType it_current = it_last_;
      bool it_reset = true;
      for(; it_current != end && ret.size() < max_size; ++it_current)
      {
        if(check_add(*it_current, point, ret)) {
          it_reset = false;
          it_last_ = it_current;
          break;
        }
      }
      if(!it_reset)
        continue;
      for(it_current = begin;
          it_current != it_last_ && ret.size() < max_size;
          ++it_current)
      {
        if(check_add(*it_current, point, ret)) {
          it_reset = false;
          it_last_ = it_current;
          break;
        }
      }
      assert(!it_reset);
    }
    return ret;
  }

private:
  const Dune::GridView<ViewTraits>& gridview_;
  IteratorType it_last_;
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

  template <class QuadpointContainerType>
  typename BaseType::EntityPointerVector
  operator() (const QuadpointContainerType& quad_points) const
  {
    auto level = std::min(gridview_.grid().maxLevel(), start_level_);
    auto range = DSC::viewRange(gridview_.grid().levelView(level));
    return process(quad_points, range);
  }

private:

  template <class QuadpointContainerType, class RangeType>
  std::vector<typename BaseType::EntityType::EntityPointer>
  process(const QuadpointContainerType& quad_points,
          const RangeType& range) const
  {
    typedef GenericReferenceElements< typename BaseType::LocalCoordinateType::value_type,
        BaseType::LocalCoordinateType::dimension > RefElementType;
    std::vector<typename BaseType::EntityType::EntityPointer> ret;

    for(const auto& my_ent : range) {
      const int my_level = my_ent.level();
      const auto& geometry = my_ent.geometry();
      const auto& refElement = RefElementType::general(geometry.type());
      for(const auto& point : quad_points)
      {
        if(refElement.checkInside(geometry.local(point)))
        {
          //if I cannot descend further add this entity even if it's not my view
          if(gridview_.grid().maxLevel() <= my_level || gridview_.contains(my_ent)) {
            ret.emplace_back(my_ent);
          }
          else {
            const auto h_end = my_ent.hend(my_level+1);
            const auto h_begin = my_ent.hbegin(my_level+1);
            const auto h_range = boost::make_iterator_range(h_begin, h_end);
            const auto kids = process(QuadpointContainerType(1, point), h_range);
            ret.insert(ret.end(), kids.begin(), kids.end());
          }
        }
      }
    }
    return ret;
  }
};

template <template <class> class SearchStrategy = DefaultSearchStrategy>
class HeterogenousProjection {

public:
  template < class SourceDFImp, class TargetDFImp >
  static void project(const Dune::DiscreteFunctionInterface<SourceDFImp>& source,
                      Dune::DiscreteFunctionInterface<TargetDFImp>& target)
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
  //! type of Lagrange point set
  typedef typename TargetDFImp::DiscreteFunctionSpaceType::LagrangePointSetType
    LagrangePointSetType;
  const auto& space =  target.space();

  // set all DoFs to infinity
  const auto dend = target.dend();
  for( auto dit = target.dbegin(); dit != dend; ++dit )
    *dit = std::numeric_limits< DofType >::infinity();

  const Iterator endit = space.end();
  for(Iterator it = space.begin(); it != endit ; ++it)
  {
    // get entity
    const EntityType& en = *it;

    // get geometry
    const auto& geo = en.geometry();

    // get local function of destination
    LocalFuncType target_lf = target.localFunction(en);

    const LagrangePointSetType& lagrangePointSet = space.lagrangePointSet(*it);
    const int quadNop = lagrangePointSet.nop();

    typename DiscreteFunctionSpaceType::RangeType value;

    std::vector<typename DiscreteFunctionSpaceType::DomainType> global_quads(quadNop);
    for(int qP = 0; qP < quadNop ; ++qP) {
      global_quads[qP] = geo.global(lagrangePointSet.point(qP));
    }
    const auto possible_entities = search(global_quads);
    assert(possible_entities.size() == global_quads.size());

    int k = 0;
    for(int qP = 0; qP < quadNop ; ++qP)
    {
      if(target_lf[ k ] == std::numeric_limits< DofType >::infinity())
      {
        const auto& global_point = global_quads[qP];

        // evaluate source function
        const auto ent_p = possible_entities[qP];
        const auto& p_geometry = ent_p->geometry();
        const auto& p_local = p_geometry.local(global_point);
        const auto& p_local_function = source.localFunction(*ent_p);
        p_local_function.evaluate(p_local, value);
        for(int i = 0; i < dimRange; ++i, ++k)
          target_lf[k] = value[i];
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

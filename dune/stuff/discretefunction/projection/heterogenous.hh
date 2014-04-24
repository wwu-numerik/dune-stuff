// This file is part of the dune-stuff project:
//   https://users.dune-project.org/projects/dune-stuff
// Copyright holders: Rene Milk, Felix Schindler
// License: BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)
//
// Contributors: Sven Kaulmann

#ifndef DUNE_STUFF_DISCRETEFUNCTION_PROJECTION_HETEROGENOUS_HH
#define DUNE_STUFF_DISCRETEFUNCTION_PROJECTION_HETEROGENOUS_HH

#include <dune/common/fvector.hh>
#include <dune/grid/common/backuprestore.hh>
#include <dune/grid/common/grid.hh>
#include <dune/grid/common/entity.hh>
#include <dune/stuff/common/ranges.hh>
#include <dune/stuff/aliases.hh>
#include <dune/stuff/fem/namespace.hh>

#if HAVE_DUNE_FEM
  #include <dune/fem/function/common/discretefunction.hh>
  #include <dune/fem/quadrature/cachingquadrature.hh>
  #include <dune/fem/space/finitevolume.hh>
  #include <dune/fem/space/lagrange.hh>
#endif

#include <dune/grid/io/file/vtk/function.hh>

#include <dune/stuff/grid/search.hh>

namespace Dune {
namespace Stuff {


#if HAVE_DUNE_FEM

template< class F, class G, int p, template< class > class S >
std::vector<typename Dune::Fem::LagrangeDiscreteFunctionSpace<F,G,p,S>::DomainType>
global_evaluation_points(const Dune::Fem::LagrangeDiscreteFunctionSpace<F,G,p,S>& space,
         const typename Dune::Fem::LagrangeDiscreteFunctionSpace<F,G,p,S>::EntityType& target_entity)
{
  const auto& target_lagrangepoint_set = space.lagrangePointSet(target_entity);
  const auto& target_geometry = target_entity.geometry();
  const auto quadNop = target_lagrangepoint_set.nop();
  std::vector<typename Dune::Fem::LagrangeDiscreteFunctionSpace<F,G,p,S>::DomainType> points(quadNop);
  for(size_t qP = 0; qP < quadNop ; ++qP) {
    points[qP] = target_geometry.global(target_lagrangepoint_set.point(qP));
  }
  return points;
}

template< class F, class G, int p, template< class > class S >
std::vector<typename Dune::Fem::FiniteVolumeSpace<F,G,p,S>::DomainType>
global_evaluation_points(const Dune::Fem::FiniteVolumeSpace<F,G,p,S>& /*space*/,
         const typename Dune::Fem::FiniteVolumeSpace<F,G,p,S>::EntityType& target_entity)
{
  assert(false);
  typedef std::vector<typename Dune::Fem::FiniteVolumeSpace<F,G,p,S>::DomainType> Ret;
  return Ret(1, target_entity.geometry().center());
}

template< template< class > class SearchStrategy = Grid::EntityInlevelSearch >
class HeterogenousProjection
{
public:
  /** If your SearchStrategy only takes a leafview of the source grid, you may use this signature.
   * Otherwise you'll have to provide an instance of the SearchStrategy to the method below
   **/
  template < class SourceDFImp, class TargetDFImp >
  static void project(const Dune::Fem::DiscreteFunctionInterface<SourceDFImp>& source,
                      Dune::Fem::DiscreteFunctionInterface<TargetDFImp>& target)
  {
    SearchStrategy<typename SourceDFImp::GridType::LeafGridView> search(source.gridPart().grid().leafView());
    project(source, target, search);
  }

  //! signature for non-default SearchStrategy constructions
  template < class SourceDFImp, class TargetDFImp, class SearchStrategyImp >
  static void project(const Dune::Fem::DiscreteFunctionInterface<SourceDFImp>& source,
                      Dune::Fem::DiscreteFunctionInterface<TargetDFImp>& target,
                      SearchStrategyImp& search)
  {
    typedef typename TargetDFImp::DiscreteFunctionSpaceType TargetDiscreteFunctionSpaceType;
    static const int target_dimRange = TargetDiscreteFunctionSpaceType::dimRange;

    const auto& space =  target.space();

    // set all DoFs to infinity
    preprocess(target);

    const auto endit = space.end();
    for(auto it = space.begin(); it != endit ; ++it)
    {
      const auto& target_entity = *it;
      auto target_local_function = target.localFunction(target_entity);
      const auto global_quads = global_evaluation_points(space, target_entity);
      const auto evaluation_entity_ptrs = search(global_quads);
      assert(evaluation_entity_ptrs.size() >= global_quads.size());

      int k = 0;
      typename TargetDiscreteFunctionSpaceType::RangeType source_value;
      for(size_t qP = 0; qP < global_quads.size() ; ++qP)
      {
        if(std::isinf(target_local_function[ k ]))
        {
          const auto& source_entity_unique_ptr = evaluation_entity_ptrs[qP];
          if (source_entity_unique_ptr) {
            const auto& source_entity_ptr = (*source_entity_unique_ptr);
            const auto& source_geometry = source_entity_ptr->geometry();
            const auto& global_point = global_quads[qP];
            const auto& source_local_point = source_geometry.local(global_point);
            const auto& source_local_function = source.localFunction(*source_entity_ptr);
            source_local_function.evaluate(source_local_point, source_value);
            for(int i = 0; i < target_dimRange; ++i, ++k)
              setDofValue(target_local_function[k], source_value[i]);
          }
          else {
            DUNE_THROW(InvalidStateException, "Did not find the local lagrange point in the source mesh!");
          }
        }
        else
          k += target_dimRange;
      }
    }
    postprocess(target);

  } // ... project(...)

protected:
  template<class TargetDFImp>
  static void preprocess(Dune::Fem::DiscreteFunctionInterface<TargetDFImp>& func) {
    typedef typename TargetDFImp::DofType TargetDofType;
    const auto infinity = DSC::numeric_limits< TargetDofType >::infinity();
    // set all DoFs to infinity
    const auto dend = func.dend();
    for( auto dit = func.dbegin(); dit != dend; ++dit )
      *dit = infinity;
  }

  template<class DofType, class SourceType >
  static void setDofValue(DofType& dof, const SourceType& value)
  {
    dof = value;
  }

  template<class TargetDFImp>
  static void postprocess(typename Dune::Fem::DiscreteFunctionInterface<TargetDFImp>& /*func*/) { return; }

}; // class HeterogenousProjection

template< template< class > class SearchStrategy = Grid::EntityInlevelSearch >
class MsFEMProjection {
public:
  //! signature for non-default SearchStrategy constructions
  template < class SourceDFImp, class TargetDFImp, class SearchStrategyImp >
  static void project(const Dune::Fem::DiscreteFunctionInterface<SourceDFImp>& source,
                      Dune::Fem::DiscreteFunctionInterface<TargetDFImp>& target,
                      SearchStrategyImp& search)
  {
    typedef typename TargetDFImp::DiscreteFunctionSpaceType TargetDiscreteFunctionSpaceType;
    static const int target_dimRange = TargetDiscreteFunctionSpaceType::dimRange;

    const auto& space =  target.space();

    // set all DoFs to infinity
    preprocess(target);

    const auto endit = space.end();
    for(auto it = space.begin(); it != endit ; ++it)
    {
      const auto& target_entity = *it;
      auto target_local_function = target.localFunction(target_entity);
      const auto global_quads = global_evaluation_points(space, target_entity);
      const auto evaluation_entity_ptrs = search(global_quads);
      assert(evaluation_entity_ptrs.size() >= global_quads.size());

      int k = 0;
      typename TargetDiscreteFunctionSpaceType::RangeType source_value;
      for(size_t qP = 0; qP < global_quads.size() ; ++qP)
      {
          const auto& source_entity_unique_ptr = evaluation_entity_ptrs[qP];
          if (source_entity_unique_ptr) {
            const auto& source_entity_ptr = (*source_entity_unique_ptr);
            const auto& source_geometry = source_entity_ptr->geometry();
            const auto& global_point = global_quads[qP];
            const auto& source_local_point = source_geometry.local(global_point);
            const auto& source_local_function = source.localFunction(*source_entity_ptr);
            source_local_function.evaluate(source_local_point, source_value);
            for(int i = 0; i < target_dimRange; ++i, ++k)
              setDofValue(target_local_function[k], source_value[i]);
          }
          else {
            DUNE_THROW(InvalidStateException, "Did not find the local lagrange point in the source mesh!");
          }
        }
      }

    postprocess(target);

  } // ... project(...)

protected:
  template<class TargetDFImp>
  static void preprocess(Dune::Fem::DiscreteFunctionInterface<TargetDFImp>& func) {

    // set all DoFs to zero
    const auto dend = func.dend();
    for( auto dit = func.dbegin(); dit != dend; ++dit )
      *dit = 0.0;
  }

  template<class DofType, class SourceType >
  static void setDofValue(DofType& dof, const SourceType& value)
  {
    dof += value;
  }

  template<class TargetDFImp>
  static void postprocess(typename Dune::Fem::DiscreteFunctionInterface<TargetDFImp>& func) {
    // compute node to entity relations
    typedef Dune::Fem::DiscreteFunctionInterface<TargetDFImp> DiscFuncType;
    const static int dimension = DiscFuncType::DiscreteFunctionSpaceType::GridPartType::GridType::dimension;
    std::vector<int> nodeToEntity(func.space().gridPart().grid().size(dimension), 0);
    identifySharedNodes(func.space().gridPart(), nodeToEntity);

    const auto dend = func.dend();
    auto factorsIt = nodeToEntity.begin();
    for (auto dit = func.dbegin(); dit!=dend; ++dit) {
      assert(factorsIt!=nodeToEntity.end());
      assert(*factorsIt>0);
      *dit /= *factorsIt;
      ++factorsIt;
    }
    return;
  }

  template<class GridPartType, class MapType>
  static void identifySharedNodes(const GridPartType& gridPart, MapType& map) {
    typedef typename GridPartType::GridType GridType;
    const auto& indexSet = gridPart.indexSet();

    for (auto& entity : DSC::viewRange(gridPart.grid().leafGridView())) {
      int number_of_nodes_in_entity = entity.template count<GridType::dimension>();
      for (int i = 0; i < number_of_nodes_in_entity; ++i) {
        const auto node = entity.template subEntity<GridType::dimension>(i);
        const auto global_index_node = indexSet.index(*node);

        // make sure we don't access non-existing elements
        assert(map.size() > global_index_node);
        ++map[global_index_node];
      }
    }
  }

};
#endif // HAVE_DUNE_FEM


} // namespace Stuff
} // namespace Dune

#endif // DUNE_STUFF_DISCRETEFUNCTION_PROJECTION_HETEROGENOUS_HH

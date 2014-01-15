#ifndef DUNE_STUFF_DISCRETEFUNCTION_PROJECTION_HETEROGENOUS_HH
#define DUNE_STUFF_DISCRETEFUNCTION_PROJECTION_HETEROGENOUS_HH

#include <dune/common/fvector.hh>
#include <dune/grid/common/backuprestore.hh>
#include <dune/grid/common/grid.hh>
#include <dune/grid/common/entity.hh>
#include <dune/stuff/common/ranges.hh>
#include <dune/stuff/aliases.hh>
#include <dune/stuff/fem/namespace.hh>

#ifdef HAVE_DUNE_FEM
  #include <dune/fem/function/common/discretefunction.hh>
  #include <dune/fem/quadrature/cachingquadrature.hh>
#endif

#include <dune/grid/io/file/vtk/function.hh>

#include <dune/stuff/grid/search.hh>

namespace Dune {
namespace Stuff {


#if 1 //def 1 // HAVE_DUNE_FEM
template< template< class > class SearchStrategy = Grid::EntityInlevelSearch >
class HeterogenousProjection
{
public:
  template < class SourceDFImp, class TargetDFImp >
  static void project(const Dune::Fem::DiscreteFunctionInterface<SourceDFImp>& source,
                      Dune::Fem::DiscreteFunctionInterface<TargetDFImp>& target)
  {
    typedef SearchStrategy<typename SourceDFImp::GridType::LeafGridView> SearchStrategyType;
    typedef typename TargetDFImp::DiscreteFunctionSpaceType TargetDiscreteFunctionSpaceType;
    typedef typename TargetDFImp::DofType TargetDofType;
    static const int target_dimRange = TargetDiscreteFunctionSpaceType::dimRange;

    const auto& space =  target.space();
    const auto infinity = DSC::numeric_limits< TargetDofType >::infinity();

    // set all DoFs to infinity
    const auto dend = target.dend();
    for( auto dit = target.dbegin(); dit != dend; ++dit )
      *dit = infinity;

    SearchStrategyType search(source.gridPart().grid().leafView());
    const auto endit = space.end();
    for(auto it = space.begin(); it != endit ; ++it)
    {
      const auto& target_entity = *it;
      const auto& target_geometry = target_entity.geometry();
      auto target_local_function = target.localFunction(target_entity);
      const auto& target_lagrangepoint_set = space.lagrangePointSet(target_entity);
      const auto quadNop = target_lagrangepoint_set.nop();

      typename TargetDiscreteFunctionSpaceType::RangeType source_value;

      std::vector<typename TargetDiscreteFunctionSpaceType::DomainType> global_quads(quadNop);
      for(size_t qP = 0; qP < quadNop ; ++qP) {
        global_quads[qP] = target_geometry.global(target_lagrangepoint_set.point(qP));
      }
      const auto evaluation_entity_ptrs = search(global_quads);
      assert(evaluation_entity_ptrs.size() >= global_quads.size());

      int k = 0;
      for(size_t qP = 0; qP < quadNop ; ++qP)
      {
        if(std::isinf(target_local_function[ k ]))
        {
          const auto& global_point = global_quads[qP];
          // evaluate source function
          const auto& source_entity_unique_ptr = evaluation_entity_ptrs[qP];
          if (source_entity_unique_ptr) {
            const auto& source_entity_ptr = (*source_entity_unique_ptr);
            const auto& source_geometry = source_entity_ptr->geometry();
            const auto& source_local_point = source_geometry.local(global_point);
            const auto& source_local_function = source.localFunction(*source_entity_ptr);
            source_local_function.evaluate(source_local_point, source_value);
            for(int i = 0; i < target_dimRange; ++i, ++k)
              target_local_function[k] = source_value[i];
          }
          else {
            for(int i = 0; i < target_dimRange; ++i, ++k)
              target_local_function[k] = TargetDofType(0);
          }
        }
        else
          k += target_dimRange;
      }
    }
  } // ... project(...)
}; // class HeterogenousProjection
#endif // HAVE_DUNE_FEM


} // namespace Stuff
} // namespace Dune

#endif // DUNE_STUFF_DISCRETEFUNCTION_PROJECTION_HETEROGENOUS_HH

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
#include <dune/grid/common/backuprestore.hh>
#include <dune/grid/common/grid.hh>
#include <dune/grid/common/gridview.hh>
#include <dune/grid/common/entity.hh>

#ifdef HAVE_DUNE_FEM
 #include <dune/fem/function/common/discretefunction.hh>
#endif

namespace Dune {
namespace Stuff {

template <class GridView>
class DefaultSearchStrategy {

  typedef typename GridView::EntityType EntityType;

  int countInside(const std::vector<typename GridView::CoordType>& corners,
                  const EntityType& entity) {
    int ret = 0;
    const auto& geometry = entity.geometry();
    const auto& refElement
      = GenericReferenceElements< DomainFieldType, dimLocal >::general(geometry.type());
    for(const auto corner : corners) {
      const LocalCoordinateType xlocal = geometry.local(corner);
      ret += refElement.checkInside(xlocal);
    }
    return ret;
  }

  const GridView& gridview_;

public:

  DefaultSearchStrategy(const Dune::Grid<GridImp>& grid)
    : grid_(grid)
  {}

  static std::vector<CoordType> getCorners(const Dune::Entity<ForeignEntityImp>& other) {
    std::vector<CoordType> ret = boost.assign.magic(other.corners());
  }

  template < class ForeignEntityImp>
  std::vector<typename EntityType::EntityPointer> operator() (const Dune::Entity<ForeignEntityImp>& other) const
  {
    std::vector<typename EntityType::EntityPointer> ret;
    auto corners = getCorners(other);
    for(const auto& my_ent : gridview_.grid().template ilevelbegin<0>(0)) {
      const int my_level = my_ent.level();
      const int inside = countInside(corners, my_ent);
      if (inside > 0) {
        //if I cannot descend further add this entity even if it's not my view
        if(gridview_.grid().maxLevel() <= my_level || gridview_.contains(my_ent)) {
          ret.push_back(EntityPointer(my_ent));
        }
        else {
          for(const auto& child : my_ent.hbegin(my_level+1)) {
            ret.extend(this->operator ()(child));
          }
        }
      }
    }
    return ret;
  }
};

template <class SearchStrategy = DefaultSearchStrategy>
class HeterogenousProjection {

  template <class EntityImp>
  int selectEntity(const CoordType& point, const std::vector<Dune::Entity<EntityImp> >& entities)
  {
    for(int i = 0; i < entities.size(); ++i) {
      const auto& entity = entities[i];
      if(entity.geometry().checkInside(point))
        return i;
    }
    return -1;
  }

  template < class SourceDF, class TargetDF >
  void project(const SourceDF& source, const TargetDF& target)
  {

  }

  template < class SourceDFImp, class TargetDFImp >
  static void project(const Dune::Fem::DiscreteFunctionInterface<SourceDFImp>& source,
               const Dune::Fem::DiscreteFunctionInterface<TargetDFImp>& target)
{

  SearchStrategy search(source.gridPart().grid().leafView());

  typedef typename DiscreteFunctionImp::DiscreteFunctionSpaceType DiscreteFunctionSpaceType;
  typedef typename DiscreteFunctionImp::LocalFunctionType LocalFuncType;
  typedef typename DiscreteFunctionSpaceType::Traits::GridPartType GridPartType;
  typedef typename DiscreteFunctionSpaceType::Traits::IteratorType Iterator;
  typedef typename DiscreteFunctionSpaceType::BaseFunctionSetType BaseFunctionSetType ;
  typedef typename GridPartType::GridType GridType;

  typedef typename FunctionImp::LocalFunctionType LocalFType;

  const DiscreteFunctionSpaceType& space =  discFunc.space();

  // type of quadrature
  typedef CachingQuadrature<GridPartType,0> QuadratureType;
  // type of local mass matrix
  typedef LocalMassMatrix< DiscreteFunctionSpaceType, QuadratureType > LocalMassMatrixType;

  const int quadOrd = (polOrd == -1) ? (2 * space.order()) : polOrd;

  // create local mass matrix object
  LocalMassMatrixType massMatrix(space, quadOrd);

  // clear destination
  discFunc.clear();

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
    LocalFuncType lf = discFunc.localFunction(en);
    // get local function of argument
    const LocalFType f = func.localFunction(en);

    const int quadNop = quad.nop();

    typename DiscreteFunctionSpaceType :: RangeType value ;

    for(int qP = 0; qP < quadNop ; ++qP)
    {
      const auto point = quad.point(qP);
      const double intel =
           quad.weight(qP) * geo.integrationElement(point);

      const auto global_point = geo.global(point);
      // evaluate function
      for(auto p : possible_entities) {

        f.evaluate(quad[ qP ], value );
      }

      // apply weight
      value *= intel;

      // add to local function
      lf.axpy( quad[ qP ], value );
    }
    massMatrix.applyInverse( en, lf );
  }
}
};

} // namespace Stuff
} // namespace Dune

#endif // DUNE_STUFF_DISCRETEFUNCTION_PROJECTION_DIRICHLET_HH

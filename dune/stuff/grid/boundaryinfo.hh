
#ifndef DUNE_STUFF_GRID_BOUNDARYINFO_HH
#define DUNE_STUFF_GRID_BOUNDARYINFO_HH

#ifdef HAVE_CMAKE_CONFIG
 #include "cmake_config.h"
#elif defined (HAVE_CONFIG_H)
 #include <config.h>
#endif // ifdef HAVE_CMAKE_CONFIG

// system
#include <map>
#include <set>
#include <string>

// dune-common
#include <dune/common/shared_ptr.hh>

namespace Dune {
namespace Stuff {
namespace Grid {

class AllDirichletBoundaryInfo
{
public:
  template< class IntersectionType >
  bool dirichlet(const IntersectionType& intersection) const
  {
    return true;
  }
}; // class AllDirichlet

class IdBasedBoundaryInfo
{
public:
  typedef int IdType;

  typedef std::set< IdType > IdSetType;

  typedef std::map< std::string, IdSetType > IdSetMapType;

  IdBased(const Dune::shared_ptr< const IdSetMapType > boundaryInfoMap)
    : boundaryInfoMap_(boundaryInfoMap)
    , hasDirichlet_(boundaryInfoMap_->find("dirichlet") != boundaryInfoMap_->end())
  {}

  template< class IntersectionType >
  bool dirichlet(const IntersectionType& intersection) const
  {
    if (hasDirichlet_ && intersection.boundary()) {
      // get boundary id
      const IdType boundaryId = intersection.boundaryId();
      // get set of dirichlet ids (has to be found, otherwise hasDirichlet_ would be false)
      const typename IdSetMapType::const_iterator result = boundaryInfoMap_->find("dirichlet");
      assert(result != boundaryInfoMap_->end());
      const IdSetType& idSet = result->second;
      return (idSet.count(boundaryId) > 0);
    } else
      return false;
  } // bool dirichlet(const IntersectionType& intersection) const

private:
  const Dune::shared_ptr< const IdSetMapType > boundaryInfoMap_;
  const bool hasDirichlet_;
}; // class IdBased


} // namespace Grid
} // namespace Stuff
} // namespace Dune

#endif // DUNE_STUFF_GRID_BOUNDARYINFO_HH

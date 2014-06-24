// This file is part of the dune-stuff project:
//   https://users.dune-project.org/projects/dune-stuff
// Copyright holders: Rene Milk, Felix Schindler
// License: BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)

#ifndef DUNE_STUFF_RESTRICT_PROLONG_HH
#define DUNE_STUFF_RESTRICT_PROLONG_HH

#if HAVE_DUNE_FEM

#include <set>

#include <dune/stuff/common/disable_warnings.hh>
# include <dune/fem/space/common/restrictprolonginterface.hh>
#include <dune/stuff/common/reenable_warnings.hh>

#include <dune/stuff/fem/namespace.hh>

namespace Dune {
namespace Stuff {
namespace Fem {

/** \brief an arbitrary size set of RestrictProlong operators of same type
   * This class is useful if you have more than a Pair of discrete functions that
   * need to be restricted/prolonged via some AdaptionManger.\n
   * All interface mandated functions are simply sequentially forwarded to each element in the set.
   * \note This set does not take ownership of added operators.
   */
template< class RestrictProlongOperatorPointerType >
class RestrictProlongOperatorSet
  : public Dune::Fem::RestrictProlongInterface< Dune::Fem::RestrictProlongTraits<
                                       RestrictProlongOperatorSet< RestrictProlongOperatorPointerType >, double > >
{
public:
  typedef typename RestrictProlongOperatorPointerType::element_type::DomainFieldType
    DomainFieldType;

  RestrictProlongOperatorSet()
    : pair_set_( SetType() )
  {}

  void setFatherChildWeight(const DomainFieldType& val) const {
    for (auto el : pair_set_)
    {
      el->setFatherChildWeight(val);
    }
  } // setFatherChildWeight

  //! restrict data to father
  template< class EntityType>
  void restrictLocal(const EntityType& father,
                     const EntityType& son,
                     bool initialize) const {
    for (const auto& el : pair_set_)
    {
      el->restrictLocal(father, son, initialize);
    }
  } // restrictLocal

  //! prolong data to children
  template< class EntityType>
  void prolongLocal(const EntityType& father,
                    const EntityType& son,
                    bool initialize) const {
    for (const auto& el : pair_set_)
    {
      el->prolongLocal(father, son, initialize);
    }
  } // prolongLocal

  //! prolong data to children
  template< class CommunicatorImp >
  void addToList(CommunicatorImp& comm) {
    for (auto el : pair_set_)
    {
      el->addToList(comm);
    }
  } // addToList

  void add(RestrictProlongOperatorPointerType rpair) {
    pair_set_.insert(rpair);
  }

  bool remove(RestrictProlongOperatorPointerType rpair) {
    // we use this erase signature so that only one of possibly multiple instances gets remvod
    auto it = pair_set_.find(rpair);
    bool found = it != pair_set_.end();

    if (found)
      pair_set_.erase(it);
    return found;
  } // remove

protected:
  typedef std::set< RestrictProlongOperatorPointerType > SetType;
  SetType pair_set_;
};

} // namespace Fem
} // namespace Stuff
} // namespace Dune

#endif // HAVE_DUNE_FEM

#endif // DUNE_STUFF_RESTRICT_PROLONG_HH

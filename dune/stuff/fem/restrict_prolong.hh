#ifndef DUNE_STUFF_RESTRICT_PROLONG_HH
#define DUNE_STUFF_RESTRICT_PROLONG_HH

#include <set>
#include <dune/fem/space/common/restrictprolonginterface.hh>

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
  : public RestrictProlongInterface< RestrictProlongTraits<
                                       RestrictProlongOperatorSet< RestrictProlongOperatorPointerType >, double > >
{
public:
  typedef typename RestrictProlongOperatorPointerType::element_type::DomainFieldType
  DomainFieldType;
  RestrictProlongOperatorSet()
    : pair_set_( SetType() )
  {}

  void setFatherChildWeight(const DomainFieldType& val) const {
    for (SetConstIteratorType it = pair_set_.begin();
         it != pair_set_.end();
         ++it)
    {
      (*it)->setFatherChildWeight(val);
    }
  } // setFatherChildWeight

  //! restrict data to father
  template< class EntityType >
  void restrictLocal(EntityType& father,
                     EntityType& son,
                     bool initialize) const {
    for (SetConstIteratorType it = pair_set_.begin();
         it != pair_set_.end();
         ++it)
    {
      (*it)->restrictLocal(father, son, initialize);
    }
  } // restrictLocal

  //! prolong data to children
  template< class EntityType >
  void prolongLocal(EntityType& father,
                    EntityType& son,
                    bool initialize) const {
    for (SetConstIteratorType it = pair_set_.begin();
         it != pair_set_.end();
         ++it)
    {
      (*it)->prolongLocal(father, son, initialize);
    }
  } // prolongLocal

  //! prolong data to children
  template< class CommunicatorImp >
  void addToList(CommunicatorImp& comm) {
    for (typename SetType::iterator it = pair_set_.begin();
         it != pair_set_.end();
         ++it)
    {
      RestrictProlongOperatorPointerType k = (*it);

      k->addToList(comm);
    }
  } // addToList

  void add(RestrictProlongOperatorPointerType rpair) {
    pair_set_.insert(rpair);
  }

  bool remove(RestrictProlongOperatorPointerType rpair) {
    // we use this erase signature so that only one of possibly multiple instances gets remvod
    SetIteratorType it = pair_set_.find(rpair);
    bool found = it != pair_set_.end();

    if (found)
      pair_set_.erase(it);
    return found;
  } // remove

protected:
  typedef std::set< RestrictProlongOperatorPointerType >
  SetType;
  typedef typename SetType::iterator
  SetIteratorType;
  typedef typename SetType::const_iterator
  SetConstIteratorType;
  SetType pair_set_;
};

} // namespace Fem
} // namespace Stuff
} // namespace Dune

#endif // DUNE_STUFF_RESTRICT_PROLONG_HH

/** Copyright (c) 2012, Rene Milk
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

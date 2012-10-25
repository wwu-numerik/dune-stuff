#ifndef DUNE_STUFF_ADAPTION_HH
#define DUNE_STUFF_ADAPTION_HH

#ifdef HAVE_CMAKE_CONFIG
 #include "cmake_config.h"
#else
 #include "config.h"
#endif // ifdef HAVE_CMAKE_CONFIG

#if HAVE_DUNE_FEM

#include <dune/stuff/fem/restrict_prolong.hh>
#include <dune/fem/space/dgspace/localrestrictprolong.hh>
#include <dune/common/shared_ptr.hh>
#include <boost/shared_ptr.hpp>

namespace Dune {
namespace Stuff {
namespace Fem {

/**
   *  \brief encapsulates the adaption handling for our DiscreteOseenFunctionWrapper
   *	Each instance produces its own RestrictProlongPair of operators for one stokesFunctionWrapper, these are in turn
   *managed
   * in a static operator set which in turn is the real argument type for the shared Dune::AdaptationManager instance.
   * \note this will horribly fail if you have FunctionWrappers of same type living on different grid instances (which
   *could be remedied by keeping a grid instance indexed map of adaption managers instead).
   *
   **/
template< class DiscreteOseenFunctionWrapperImp >
class DiscreteOseenFunctionWrapperAdaptionManager
{
protected:
  typedef typename DiscreteOseenFunctionWrapperImp::GridType
  GridType;

  typedef Dune::RestrictProlongDefault< typename DiscreteOseenFunctionWrapperImp::DiscretePressureFunctionType >
  RestrictProlongPressureType;
  typedef Dune::AdaptationManager< GridType, RestrictProlongPressureType >
  PressureAdaptationManagerType;

  typedef Dune::RestrictProlongDefault< typename DiscreteOseenFunctionWrapperImp::DiscreteVelocityFunctionType >
  RestrictProlongVelocityType;
  typedef Dune::AdaptationManager< GridType, RestrictProlongVelocityType >
  VelocityAdaptationManagerType;

  typedef Dune::RestrictProlongPair< RestrictProlongVelocityType&, RestrictProlongPressureType& >
  RestrictProlongPairType;
  typedef boost::shared_ptr< RestrictProlongPairType >
  RestrictProlongPairPointerType;
  typedef RestrictProlongOperatorSet< RestrictProlongPairPointerType >
  RestrictProlongOperatorSetType;

  typedef Dune::AdaptationManager< GridType, RestrictProlongOperatorSetType >
  AdaptationManagerType;

  typedef boost::shared_ptr< AdaptationManagerType >
  AdaptationManagerPointerType;

public:
  DiscreteOseenFunctionWrapperAdaptionManager(GridType& grid,
                                              DiscreteOseenFunctionWrapperImp& functionWrapper)
    : grid_(grid)
      , function_wrapper_(functionWrapper)
      , rpVelocity_( functionWrapper.discreteVelocity() )
      , rpPressure_( functionWrapper.discretePressure() )
      , restrictPair_ptr_( new RestrictProlongPairType(rpVelocity_, rpPressure_) ) {
    combined_adaptManager_ptr_ = getAdaptationManager();
    assert(combined_adaptManager_ptr_);
    restrictOperator_Set_.add(restrictPair_ptr_);
  }

  AdaptationManagerPointerType getAdaptationManager() {
    static AdaptationManagerPointerType ptr( new AdaptationManagerType(grid_, restrictOperator_Set_) );

    return ptr;
  }

  ~DiscreteOseenFunctionWrapperAdaptionManager() {
    restrictOperator_Set_.remove(restrictPair_ptr_);
  }

  void adapt() {
    assert(combined_adaptManager_ptr_);
    combined_adaptManager_ptr_->adapt();
  }

  DiscreteOseenFunctionWrapperAdaptionManager(DiscreteOseenFunctionWrapperAdaptionManager& other)
    : grid_(other.grid_)
      , function_wrapper_(other.function_wrapper_)
      , rpVelocity_( function_wrapper_.discreteVelocity() )
      , rpPressure_( function_wrapper_.discretePressure() )
      , restrictPair_ptr_( new RestrictProlongPairType(rpVelocity_, rpPressure_) )
  {}

protected:
  GridType& grid_;
  DiscreteOseenFunctionWrapperImp& function_wrapper_;
  RestrictProlongVelocityType rpVelocity_;
  RestrictProlongPressureType rpPressure_;
  RestrictProlongPairPointerType restrictPair_ptr_;
  static RestrictProlongOperatorSetType restrictOperator_Set_;
  AdaptationManagerPointerType combined_adaptManager_ptr_;
};
template< class T >
typename DiscreteOseenFunctionWrapperAdaptionManager< T >::RestrictProlongOperatorSetType
DiscreteOseenFunctionWrapperAdaptionManager< T >::restrictOperator_Set_;

} // namespace Fem
} // namespace Stuff
} // namespace Dune

#endif // #if HAVE_DUNE_FEM

#endif // DUNE_STUFF_ADAPTION_HH
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

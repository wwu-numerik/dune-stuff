#ifndef STUFF_TIMEFUNCTION_HH
#define STUFF_TIMEFUNCTION_HH

#ifdef HAVE_CMAKE_CONFIG
 #include "cmake_config.h"
#else
 #include "config.h"
#endif // ifdef HAVE_CMAKE_CONFIG

#if HAVE_DUNE_FEM

#include <dune/fem/function/common/function.hh>
#include <dune/common/bartonnackmanifcheck.hh>

namespace Dune {
namespace Stuff {
namespace Fem {

//! a dummy timeprovider for usage in Stuff::RuntimeFunction
struct ConstTimeProvider
{
  const double time_;
  ConstTimeProvider(double time = 0.0)
    : time_(time) {}
  double subTime() const { return time_; }
};

/** \brief a Base class for Dune::Fem::Function compatible classes that need automatic time awareness via TimeProvider
   *
   **/
template< class FunctionSpaceImp, class FunctionImp, class TimeProviderImp >
class TimeFunction
  : public BartonNackmanInterface< TimeFunction< FunctionSpaceImp, FunctionImp, TimeProviderImp >,
                                   FunctionImp >
    , public Dune::Fem::Function< FunctionSpaceImp, TimeFunction< FunctionSpaceImp, FunctionImp, TimeProviderImp > >
{
protected:
  typedef TimeFunction< FunctionSpaceImp, FunctionImp, TimeProviderImp >
  ThisType;
  typedef Dune::Fem::Function< FunctionSpaceImp, ThisType >
  BaseType;
  typedef BartonNackmanInterface< ThisType, FunctionImp >
  Interface;

  TimeProviderImp* timeProviderPtr_;          // in case we cosntruct our own
  const TimeProviderImp& timeProvider_;

  using Interface::asImp;
  static const int dim_ = FunctionSpaceImp::dimDomain;

public:
  typedef TimeProviderImp
  TimeProviderType;

  TimeFunction(const TimeProviderImp& timeProviderImp, const FunctionSpaceImp& /*space*/)
    : BaseType()
      , timeProviderPtr_(nullptr)
      , timeProvider_(timeProviderImp)
  {}
  TimeFunction(const TimeProviderImp& timeProviderImp)
    : BaseType()
      , timeProviderPtr_(nullptr)
      , timeProvider_(timeProviderImp)
  {}
  explicit TimeFunction(double t)
    : BaseType()
      , timeProviderPtr_( new TimeProviderType(t) )
      , timeProvider_(*timeProviderPtr_)
  {}

  virtual ~TimeFunction() {
    delete timeProviderPtr_;
  }

  inline void evaluate(const typename BaseType::DomainType& x, typename BaseType::RangeType& ret) const {
    const double t = timeProvider_.subTime();

    CHECK_AND_CALL_INTERFACE_IMPLEMENTATION( asImp().evaluateTime(t, x, ret) );
  }

  inline void evaluate(double time, const typename BaseType::DomainType& x, typename BaseType::RangeType& ret) const {
    CHECK_AND_CALL_INTERFACE_IMPLEMENTATION( asImp().evaluateTime(time, x, ret) );
  }

  /** \brief evaluate the Jacobian of the function
     *
     *  \param[in]  x    evaluation point
     *  \param[out] ret  value of the Jacobian in x
     */
  void jacobian(const typename BaseType::DomainType& x,
                typename BaseType::JacobianRangeType& ret) const {
    const double t = timeProvider_.subTime();

    CHECK_AND_CALL_INTERFACE_IMPLEMENTATION( asImp().jacobianTime(t, x, ret) );
  }

  /** \brief evaluate a derivative of the function
     *
     *  \param[in]  diffVariable  vector describing the partial derivative to
     *                            evaluate
     *  \param[in]  x             evaluation point
     *  \param[out] ret           value of the derivative in x
     */
  template< int diffOrder >
  inline void evaluate(const FieldVector< int, diffOrder >& diffVariable,
                       const typename BaseType::DomainType& x,
                       typename BaseType::RangeType& ret) const {
    CHECK_AND_CALL_INTERFACE_IMPLEMENTATION
      ( asImp().evaluate(diffVariable, x, ret) );
  }

  const TimeProviderType& timeProvider() const {
    return timeProvider_;
  }
};

/** \brief basically the same as TimeFunction, but with an evaluate signature that additionally gets an
 * IntersectionIterator
   *
   **/
template< class FunctionSpaceImp, class FunctionImp, class TimeProviderImp >
class IntersectionTimeFunction
  : public BartonNackmanInterface< IntersectionTimeFunction< FunctionSpaceImp, FunctionImp, TimeProviderImp >,
                                   FunctionImp >
    , public Dune::Fem::Function< FunctionSpaceImp, IntersectionTimeFunction< FunctionSpaceImp, FunctionImp, TimeProviderImp > >
{
protected:
  typedef IntersectionTimeFunction< FunctionSpaceImp, FunctionImp, TimeProviderImp >
  ThisType;
  typedef Dune::Fem::Function< FunctionSpaceImp, ThisType >
  BaseType;
  typedef BartonNackmanInterface< ThisType, FunctionImp >
  Interface;

  const TimeProviderImp& timeProvider_;

  using Interface::asImp;
  static const int dim_ = FunctionSpaceImp::dimDomain;

public:
  typedef TimeProviderImp
  TimeProviderType;

  IntersectionTimeFunction(const TimeProviderImp& timeProviderImp, const FunctionSpaceImp& /*space*/)
    : BaseType()
      , timeProvider_(timeProviderImp)
  {}

  ~IntersectionTimeFunction()
  {}

  template< class IntersectionType >
  inline void evaluate(const typename BaseType::DomainType& x,
                       typename BaseType::RangeType& ret,
                       const IntersectionType& intersection) const {
    const double t = timeProvider_.subTime();

    CHECK_AND_CALL_INTERFACE_IMPLEMENTATION( asImp().evaluateTime(t, x, ret, intersection) );
  }

  template< class IntersectionType >
  inline void evaluate(double time,
                       const typename BaseType::DomainType& x,
                       typename BaseType::RangeType& ret,
                       const IntersectionType& intersection) const {
    CHECK_AND_CALL_INTERFACE_IMPLEMENTATION( asImp().evaluateTime(time, x, ret, intersection) );
  }

  inline void evaluate(const typename BaseType::DomainType& /*x*/, typename BaseType::RangeType& /*ret*/) const {
    DUNE_THROW(Dune::NotImplemented, "__FUNCTION__ only present for interface compat");
  }

  inline void evaluate(double time,
                       const typename BaseType::DomainType& x,
                       typename BaseType::RangeType& ret) const {
    CHECK_AND_CALL_INTERFACE_IMPLEMENTATION( asImp().evaluateTime(time, x, ret) );
  }

  /** \brief evaluate the Jacobian of the function
     *
     *  \param[in]  x    evaluation point
     *  \param[out] ret  value of the Jacobian in x
     */
  inline void jacobian(const typename BaseType::DomainType& /*x*/,
                       typename BaseType::JacobianRangeType& /*ret*/) const {
    DUNE_THROW(Dune::NotImplemented, "__CLASS__ needs to be extended");
  }

  /** \brief evaluate a derivative of the function
     *
     *  \param[in]  diffVariable  vector describing the partial derivative to
     *                            evaluate
     *  \param[in]  x             evaluation point
     *  \param[out] ret           value of the derivative in x
     */
  template< int diffOrder >
  inline void evaluate(const FieldVector< int, diffOrder >& /*diffVariable*/,
                       const typename BaseType::DomainType& /*x*/,
                       typename BaseType::RangeType& /*ret*/) const {
    DUNE_THROW(Dune::NotImplemented, "__CLASS__ needs to be extended");
  }

  const TimeProviderType& timeProvider() const {
    return timeProvider_;
  }
};

} // namespace Fem
} // namespace Stuff
} // namespace Dune

#endif //HAVE_DUNE_FEM
#endif // STUFF_TIMEFUNCTION_HH
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

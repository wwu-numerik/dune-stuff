#ifndef DUNE_STUFF_ANALYTICAL_HH
#define DUNE_STUFF_ANALYTICAL_HH

#ifdef HAVE_CMAKE_CONFIG
 #include "cmake_config.h"
#else
 #include "config.h"
#endif // ifdef HAVE_CMAKE_CONFIG

#ifdef HAVE_DUNE_FEM

#include "timefunction.hh"

namespace Dune {
namespace Stuff {
namespace Fem {

template< class FunctionSpaceImp >
class ConstantFunction
  : public Dune::Fem::Function< FunctionSpaceImp, ConstantFunction< FunctionSpaceImp > >
{
public:
  typedef ConstantFunction< FunctionSpaceImp >
    ThisType;
  typedef Dune::Fem::Function< FunctionSpaceImp, ThisType >
    BaseType;
  typedef typename BaseType::DomainType
    DomainType;
  typedef typename BaseType::RangeType
    RangeType;

  ConstantFunction(const FunctionSpaceImp& /*space*/,
                   const double constant = 0.0)
    : constant_(RangeType(constant))
  {}
  explicit ConstantFunction(const double constant = 0.0)
    : constant_(RangeType(constant))
  {}
  explicit ConstantFunction(const RangeType& constant)
    : constant_(constant)
  {}

  ~ConstantFunction()
  {}

  inline void evaluate(RangeType& ret) const {
    ret = constant_;
  }

  //! recursively consume args until only the output arg RangeType is left and the above function is called
  template <class FirstType, class... Args>
  void evaluate(FirstType&&, Args&&... args) const { evaluate(args...); }

  template< class IntersectionIteratorType >
  inline void evaluate(const double /*time*/, const DomainType& /*arg*/,
                       RangeType& ret, const IntersectionIteratorType /*it*/) const
  { ret = constant_; }

  inline void jacobian(typename BaseType::JacobianRangeType& jac) const {
    jac = typename BaseType::JacobianRangeType(0);
  }

  template <class FirstType, class... Args>
  void jacobian(FirstType&&, Args&&... args) const { jacobian(args...); }

  //! alias for evaluate
  template <class... Args>
  void position_derivative(Args&&... args) const { evaluate(std::forward<Args>(args)...); }

  //! alias for jacobian
  template <class... Args>
  void direction_derivative(Args&&... args) const { jacobian(std::forward<Args>(args)...); }

  //! deprecated alias for jacobian
  template <class... Args>
  void evaluateJacobian(Args&&... args) const { jacobian(std::forward<Args>(args)...); }

private:
  const RangeType constant_;
};

template< class FunctionSpaceImp, class TimeProviderImp >
class ConstantFunctionTP
  : public TimeFunction< FunctionSpaceImp,
                               ConstantFunctionTP< FunctionSpaceImp, TimeProviderImp >, TimeProviderImp >
{
public:
  typedef ConstantFunctionTP< FunctionSpaceImp, TimeProviderImp >
    ThisType;
  typedef TimeFunction< FunctionSpaceImp, ThisType, TimeProviderImp >
    BaseType;
  typedef typename BaseType::DomainType
    DomainType;
  typedef typename BaseType::RangeType
    RangeType;

  ConstantFunctionTP(const TimeProviderImp& timeprovider,
                     const FunctionSpaceImp& space,
                     const double constant = 0.0)
    : BaseType(timeprovider, space)
      , constant_(constant)
  {}

  ~ConstantFunctionTP()
  {}

  void evaluateTime(const double /*time*/, const DomainType& /*arg*/, RangeType& ret) const { ret = RangeType(constant_); }

private:
  const double constant_;
};

template< class FunctionSpaceImp, class TimeProviderImp >
class ConstantIntersectionTimeFunction
  : public IntersectionTimeFunction< FunctionSpaceImp,
                                           ConstantIntersectionTimeFunction< FunctionSpaceImp,
                                                                             TimeProviderImp >, TimeProviderImp >
{
public:
  typedef ConstantIntersectionTimeFunction< FunctionSpaceImp, TimeProviderImp >
    ThisType;
  typedef IntersectionTimeFunction< FunctionSpaceImp, ThisType, TimeProviderImp >
    BaseType;
  typedef typename BaseType::DomainType
    DomainType;
  typedef typename BaseType::RangeType
    RangeType;

  /**
     *  \brief  constructor
     *  \param  viscosity,alpha   dummies
     **/
  ConstantIntersectionTimeFunction(const TimeProviderImp& timeprovider,
                                   const FunctionSpaceImp& space,
                                   const double constant = 0.0)
    : BaseType(timeprovider, space)
      , constant_(constant)
  {}

  /**
     *  \brief  destructor
     *
     *  doing nothing
     **/
  ~ConstantIntersectionTimeFunction()
  {}

  template< class IntersectionType >
  void evaluateTime(const double /*time*/, const DomainType& /*arg*/, RangeType& ret,
                    const IntersectionType& /*intersection */) const {
    ret = RangeType(constant_);
  }

  void evaluateTime(const double /*time*/, const DomainType& /*arg*/, RangeType& ret) const {
    ret = RangeType(constant_);
  }

private:
  const double constant_;
};

} // end namespace Fem
} // end namespace Stuff
} // end namespace Dune

#define NULLFUNCTION_TP(classname) \
  template< class T, class P > \
  struct classname \
    : public Dune::Stuff::Fem::ConstantFunctionTP< T, P > \
  { classname(const P &p, const T &t, double = 0.0, double = 0.0) \
      : Dune::Stuff::Fem::ConstantFunctionTP< T, P >(p, t) {} };

#define NULLFUNCTION_TP_BOUNDARY(classname) \
  template< class T, class P > \
  struct classname \
    : public Dune::Stuff::Fem::ConstantIntersectionTimeFunction< T, P > \
  { classname(const P &p, const T &t, double = 0.0, double = 0.0) \
      : Dune::Stuff::Fem::ConstantIntersectionTimeFunction< T, P >(p, t) {} };

#define NULLFUNCTION(classname) \
  template< class T > \
  struct classname \
    : public Dune::Stuff::Fem::ConstantFunction< T > \
  { classname(const double /*d*/, const T &t, double = 0.0, double = 0.0) \
      : Dune::Stuff::Fem::ConstantFunction< T >(t) {} \
    classname() \
      : Dune::Stuff::Fem::ConstantFunction< T >() {} \
    classname(const T &t) \
      : Dune::Stuff::Fem::ConstantFunction< T >(t) {} };

#define CONSTANTFUNCTION(classname, constant) \
  template< class T > \
  struct classname \
    : public Dune::Stuff::Fem::ConstantFunction< T > \
  { classname() \
      : Dune::Stuff::Fem::ConstantFunction< T >(typename T::RangeType(constant)) {} };

#endif // HAVE_DUNE_FEM

#endif // DUNE_STUFF_ANALYTICAL_HH

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

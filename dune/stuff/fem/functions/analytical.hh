// This file is part of the dune-stuff project:
//   https://github.com/wwu-numerik/dune-stuff
// Copyright holders: Rene Milk, Felix Schindler
// License: BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)

#warning Will be removed soon, file an issue on https://github.com/wwu-numerik/dune-stuff/issues if you need this (09.02.2015)!

#ifndef DUNE_STUFF_ANALYTICAL_HH
#define DUNE_STUFF_ANALYTICAL_HH

#if HAVE_DUNE_FEM

#include <dune/common/deprecated.hh>

#include "timefunction.hh"

namespace Dune {
namespace Stuff {
namespace Fem {

template< class FunctionSpaceImp >
class
  DUNE_DEPRECATED_MSG("Will be removed soon, file an issue on https://github.com/wwu-numerik/dune-stuff/issues if you need this (09.02.2015)!")
      ConstantFunction
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
class
  DUNE_DEPRECATED_MSG("Will be removed soon, file an issue on https://github.com/wwu-numerik/dune-stuff/issues if you need this (09.02.2015)!")
      ConstantFunctionTP
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
class
  DUNE_DEPRECATED_MSG("Will be removed soon, file an issue on https://github.com/wwu-numerik/dune-stuff/issues if you need this (09.02.2015)!")
      ConstantIntersectionTimeFunction
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
  struct DUNE_DEPRECATED_MSG("Will be removed soon, file an issue on https://github.com/wwu-numerik/dune-stuff/issues if you need this (09.02.2015)!") classname \
    : public Dune::Stuff::Fem::ConstantFunctionTP< T, P > \
  { classname(const P &p, const T &t, double = 0.0, double = 0.0) \
      : Dune::Stuff::Fem::ConstantFunctionTP< T, P >(p, t) {} };

#define NULLFUNCTION_TP_BOUNDARY(classname) \
  template< class T, class P > \
  struct DUNE_DEPRECATED_MSG("Will be removed soon, file an issue on https://github.com/wwu-numerik/dune-stuff/issues if you need this (09.02.2015)!") classname \
    : public Dune::Stuff::Fem::ConstantIntersectionTimeFunction< T, P > \
  { classname(const P &p, const T &t, double = 0.0, double = 0.0) \
      : Dune::Stuff::Fem::ConstantIntersectionTimeFunction< T, P >(p, t) {} };

#define NULLFUNCTION(classname) \
  template< class T > \
  struct DUNE_DEPRECATED_MSG("Will be removed soon, file an issue on https://github.com/wwu-numerik/dune-stuff/issues if you need this (09.02.2015)!") classname \
    : public Dune::Stuff::Fem::ConstantFunction< T > \
  { classname(const double /*d*/, const T &t, double = 0.0, double = 0.0) \
      : Dune::Stuff::Fem::ConstantFunction< T >(t) {} \
    classname() \
      : Dune::Stuff::Fem::ConstantFunction< T >() {} \
    classname(const T &t) \
      : Dune::Stuff::Fem::ConstantFunction< T >(t) {} };

#define CONSTANTFUNCTION(classname, constant) \
  template< class T > \
  struct DUNE_DEPRECATED_MSG("Will be removed soon, file an issue on https://github.com/wwu-numerik/dune-stuff/issues if you need this (09.02.2015)!") classname \
    : public Dune::Stuff::Fem::ConstantFunction< T > \
  { classname() \
      : Dune::Stuff::Fem::ConstantFunction< T >(typename T::RangeType(constant)) {} };

#endif // HAVE_DUNE_FEM

#endif // DUNE_STUFF_ANALYTICAL_HH

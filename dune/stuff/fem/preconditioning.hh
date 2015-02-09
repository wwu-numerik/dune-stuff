// This file is part of the dune-stuff project:
//   https://github.com/wwu-numerik/dune-stuff
// Copyright holders: Rene Milk, Felix Schindler
// License: BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)

#warning Will be removed soon, file an issue on https://github.com/wwu-numerik/dune-stuff/issues if you need this (09.02.2015)!

#ifndef DUNE_STUFF_PRECONDITIONING_HH
#define DUNE_STUFF_PRECONDITIONING_HH

#include <dune/common/deprecated.hh>

namespace Dune {
namespace Stuff {
namespace Fem {

//! allow any class fullfilling the Operator concept to be used as a preconditioner
template< class Operator, template< class T, class F > class Solver, class RangeDiscreteFunctionType >
class
  DUNE_DEPRECATED_MSG("Will be removed soon, file an issue on https://github.com/wwu-numerik/dune-stuff/issues if you need this (09.02.2015)!")
      OperatorBasedPreconditioner
{
  Operator& operator_;
  const typename RangeDiscreteFunctionType::FunctionSpaceType & range_space_;
  const bool right_preconditioning_;
  typedef Solver< RangeDiscreteFunctionType, Operator > SolverType;
  SolverType solver_;

public:
  OperatorBasedPreconditioner(Operator& op,
                              const typename RangeDiscreteFunctionType::FunctionSpaceType& range_space,
                              const bool right_preconditioning = false,
                              double solver_accuracy = 1e-7)
    : operator_(op)
    , range_space_(range_space)
    , right_preconditioning_(right_preconditioning)
    , solver_(operator_,
              solver_accuracy /*rel limit*/,
              solver_accuracy /*abs limit*/,
              1 /*not working iteration limit*/,
              false /*verbose*/)
  {}

  template< class VecType >
  void precondition(const VecType* tmp, VecType* dest) const {
    multOEM(tmp, dest);
  }

  template< class VECtype >
  void multOEM(const VECtype* x, VECtype* ret) const {
    operator_.multOEM(x, ret);
  }

  bool rightPrecondition() const {
    return right_preconditioning_;
  }
};

} // namespace Fem
} // namespace Stuff
} // namespace Dune

#endif // DUNE_STUFF_PRECONDITIONING_HH

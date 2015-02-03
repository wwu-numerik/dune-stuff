// This file is part of the dune-stuff project:
//   https://github.com/wwu-numerik/dune-stuff
// Copyright holders: Rene Milk, Felix Schindler
// License: BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)

#ifndef STUFF_FUNCTION_TRANSFORM_HH
#define STUFF_FUNCTION_TRANSFORM_HH

#include <dune/stuff/common/ranges.hh>
#include <dune/stuff/aliases.hh>

namespace Dune {
namespace Stuff {
namespace Fem {

//! (inplace) multiplies given function with matrix diagonal
template< class Matrix, class Function >
void DiagonalMult(const Matrix& matrix, Function& f) {
  Function diag( "temp", f.space() );
  matrix.getDiag(diag);
  auto diag_it = diag.dbegin();
  auto f_it = f.dbegin();
  for (auto row : DSC::valueRange(matrix.size(0)))
  {
    (*f_it) *= (*diag_it);
    ++f_it;
    ++diag_it;
  }
  return;
} // DiagonalMult

//! return true if any dof is nan or inf
template< class DiscreteFunctionType >
void divideFunctionDofs(DiscreteFunctionType& target, const DiscreteFunctionType& divider) {
  auto it = target.dbegin();
  auto cit = divider.dbegin();
  for ( ; it != target.dend(); ++it, ++cit)
  {
    *it /= *cit;
  }
} // divideFunctionDofs

/** \todo RENE needs to doc me **/
template< class DiscreteFunctionType >
void invertFunctionDofs(DiscreteFunctionType& function) {
  auto it = function.dbegin();
  for ( ; it != function.dend(); ++it)
  {
    if (*it != 0.0)
      *it = 1 / (*it);
  }
  return;
} // invertFunctionDofs

//! inverts dof order
template< class Function >
void switchDofs(Function& f) {
  auto front = f.dbegin();
  auto back = f.dend();
  const auto numdofs = f.size();
  for (auto i : DSC::valueRange(numdofs / 2))
  {
    auto tmp = *back;
    *back = *front;
    *front = tmp;
  }
  return;
} // switchDofs

/**
   * \brief shift each dof by scalar
   *
   * this is the actual functions value, no multiplication with basefunctions afterwards
   **/
template< class Function >
void addScalarToFunc(Function& f, double sc) {
  auto it = f.dbegin();
  for ( ; it != f.dend(); ++it)
    *it += sc;
  return;
} // addScalarToFunc

} // namespace Dune {
} // namespace Stuff {
} // namespace Fem {

#endif // STUFF_FUNCTION_TRANSFORM_HH

// This file is part of the dune-stuff project:
//   http://users.dune-project.org/projects/dune-stuff
// Copyright holders: Rene Milk, Felix Schindler
// License: BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)

#ifndef STUFF_FUNCTION_TRANSFORM_HH
#define STUFF_FUNCTION_TRANSFORM_HH

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
  for (int row = 0; row < matrix.size(0); row++)
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
  const unsigned int numdofs = f.size();
  for (unsigned int i = 0; i < numdofs / 2; ++i)
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

/** Copyright (c) 2012, Rene Milk, Felix Schindler
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

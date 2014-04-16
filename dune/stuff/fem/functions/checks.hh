#ifndef STUFF_FUNCTIONS_HH_INCLUDED
#define STUFF_FUNCTIONS_HH_INCLUDED

#if HAVE_DUNE_FEM

#include <cmath>
#include <fstream>
#include <limits>

#include <dune/fem/io/file/vtkio.hh>
#include <dune/stuff/fem/localmassmatrix.hh>
#include <dune/stuff/common/misc.hh>
#include <dune/stuff/common/math.hh>
#include <dune/stuff/common/string.hh>
#include <dune/common/deprecated.hh>
#include <dune/common/fvector.hh>
#include <dune/common/math.hh>

namespace Dune {
namespace Stuff {
namespace Fem {

/**
   *  \brief  gets min, avg and max of a Dune::DiscreteFunction
   *
   *          or compatible in terms of iterators
   *  \attention  works only for constant base function = sqrt(2) atm
   **/
template< class FunctionType >
Dune::Stuff::Common::MinMaxAvg<typename FunctionType::RangeFieldType>
getMinMaxOfDiscreteFunction(const FunctionType& function)
{
  Dune::Stuff::Common::MinMaxAvg<typename FunctionType::RangeFieldType> mm;
  typedef typename FunctionType::ConstDofIteratorType
  ConstDofIteratorType;
  ConstDofIteratorType itEnd = function.dend();
  // find minimum and maximum
  for (ConstDofIteratorType it = function.dbegin(); it != itEnd; ++it)
  {
    mm(*it);
  }
  return mm;
}

template< class FunctionType >
void
getMinMaxOfDiscreteFunction(const FunctionType& function, const std::string name, std::ostream& out)
{
  auto mm = getMinMaxOfDiscreteFunction(function);
  out << name << " ";
  mm.output(out);
  out << std::endl;
}
/**
   *  \brief  gets min and max of a Dune::DiscreteFunction
   *
   *          or compatible in terms of iterators
   *  \attention  works only for constant base function = sqrt(2) atm
   **/
template< class FunctionType >
void getMinMaxOfDiscreteFunction(const FunctionType& function,
                                 double& min,
                                 double& max) {
  auto mm = getMinMaxOfDiscreteFunction(function);
  min = mm.min();
  max = mm.max();
} // getMinMaxOfDiscreteFunction


//! count dofs of f1,f2 with abs(f1[i] - f2[i]) > tolerance
template< class FunctionType >
unsigned int getNumDiffDofs(const FunctionType& f1,
                            const FunctionType& f2,
                            const double tolerance) {
  assert( f1.size() == f2.size() ); // should be implicit cause they're of the same type...
  unsigned int numDiffs = 0;
  auto itEnd = f1.dend();
  auto f2it = f2.dbegin();
  for (auto f1it = f1.dbegin(); f1it != itEnd; ++f1it, ++f2it)
  {
    numDiffs += (std::fabs(*f1it - *f2it) > tolerance);
  }
  return numDiffs;
} // getNumDiffDofs


//! returns arithmetic mean of function's dofs
template< class Function >
typename Function::FieldType getFuncAvg(const Function& f) {
  auto it = f.dbegin();
  const unsigned int numdofs = f.size();
  typename Function::FieldType sum = 0;
  for ( ; it != f.dend(); ++it)
    sum += *it;
  sum /= double(numdofs);
  return sum;
} // getFuncAvg

//! return true if any dof is nan or inf
template< class DiscreteFunctionType >
bool FunctionContainsNanOrInf(const DiscreteFunctionType& function) {
  auto it = function.dbegin();
  for ( ; it != function.dend(); ++it)
  {
    if ( boost::math::isnan(*it) || boost::math::isinf(*it) )
      return true;
  }
  return false;
} // FunctionContainsNanOrInf

template< class LeakPointerType >
bool FunctionContainsNanOrInf(const LeakPointerType function, size_t size) {
  for (size_t i = 0; i < size; ++i)
  {
    if ( boost::math::isnan(function[i]) || boost::math::isinf(function[i]) )
      return true;
  }
  return false;
} // FunctionContainsNanOrInf

//! return true if any entry is nan or inf
template< class MatrixType >
bool MatrixContainsNanOrInf(const MatrixType& matrix) {
  for (int row = 0; row < int( matrix.rows() ); ++row)
  {
    for (int col = 0; col < int( matrix.cols() ); ++col)
    {
      if ( std::isnan( matrix(row, col) ) || std::isinf( matrix(row, col) ) )
        return true;
    }
  }
  return false;
} // MatrixContainsNanOrInf

} // namespace Dune {
} // namespace Stuff {
} // namespace Fem {

#endif // HAVE_DUNE_FEM

#endif // includeguard
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

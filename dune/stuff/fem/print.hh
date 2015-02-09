// This file is part of the dune-stuff project:
//   https://github.com/wwu-numerik/dune-stuff
// Copyright holders: Rene Milk, Felix Schindler
// License: BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)

#warning Will be removed soon, file an issue on https://github.com/wwu-numerik/dune-stuff/issues if you need this (09.02.2015)!

#ifndef DUNE_STUFF_FEM_PRINT_HH
#define DUNE_STUFF_FEM_PRINT_HH

#include <dune/common/deprecated.hh>

#include "../common/print.hh"

#if HAVE_DUNE_FEM
# include <dune/stuff/fem/functions/checks.hh>

namespace Dune {
namespace Stuff {
namespace Common {


/** print min/max of a given DiscreteFucntion obtained by Stuff::getMinMaxOfDiscreteFunction
   * \note hardcoded mult of values by sqrt(2)
   **/
template< class Function >
void
  DUNE_DEPRECATED_MSG("Will be removed soon, file an issue on https://github.com/wwu-numerik/dune-stuff/issues if you need this (09.02.2015)!")
     printFunctionMinMax(std::ostream& stream, const Function& func) {
  double min = 0.0;
  double max = 0.0;

  Dune::Stuff::Fem::getMinMaxOfDiscreteFunction(func, min, max);
  stream << "  - " << func.name() << std::endl
         << "    min: " << std::sqrt(2.0) * min << std::endl
         << "    max: " << std::sqrt(2.0) * max << std::endl;
} // printFunctionMinMax


} // namespace Common
} // namespace Stuff
} // namespace Dune

#endif // HAVE_DUNE_FEM
#endif // ifndef DUNE_STUFF_FEM_PRINT_HH

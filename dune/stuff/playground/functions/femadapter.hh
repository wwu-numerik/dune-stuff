// This file is part of the dune-stuff project:
//   https://users.dune-project.org/projects/dune-stuff
// Copyright holders: Rene Milk, Felix Schindler
// License: BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)

#ifndef DUNE_STUFF_FUNCTION_FEMADAPTER_HH
#define DUNE_STUFF_FUNCTION_FEMADAPTER_HH


#if HAVE_DUNE_FEM

#include <dune/fem/function/common/function.hh>
#include <dune/fem/space/common/functionspace.hh>
#include <dune/fem/quadrature/quadrature.hh>

#include <dune/stuff/functions/interfaces.hh>
#include <dune/stuff/common/memory.hh>

namespace Dune {
namespace Stuff {

template <class DataType, class GridPartType>
Dune::Fem::GridFunctionAdapter<DataType, GridPartType> gridFunctionAdapter(const DataType& data, const GridPartType& grid_part)
DUNE_DEPRECATED_MSG("Use the interfaces from interfaces.hh or put this somewhere else!")
{
  return Dune::Fem::GridFunctionAdapter<DataType, GridPartType> ("", data, grid_part);
}

} // namespace Stuff
} // namespace Dune

#endif // HAVE_DUNE_FEM

#endif // DUNE_STUFF_FUNCTION_FEMADAPTER_HH

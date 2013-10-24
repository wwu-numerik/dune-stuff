// This file is part of the dune-stuff project:
//   http://users.dune-project.org/projects/dune-stuff/
// Copyright Holders: Felix Albrecht
// License: BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)

#include "config.h"

#ifdef DUNE_STUFF_FUNCTIONS_TO_LIB
#include "expression.hh"

#define DUNE_STUFF_FUNCTIONS_EXPRESSION_CC_LIST_DIMRANGE(etype, ddim) \
  DUNE_STUFF_FUNCTIONS_EXPRESSION_CC_LIST_DIMRANGECOLS(etype, ddim, 1) \
  DUNE_STUFF_FUNCTIONS_EXPRESSION_CC_LIST_DIMRANGECOLS(etype, ddim, 2) \
  DUNE_STUFF_FUNCTIONS_EXPRESSION_CC_LIST_DIMRANGECOLS(etype, ddim, 3)

#define DUNE_STUFF_FUNCTIONS_EXPRESSION_CC_LIST_DIMRANGECOLS(etype, ddim, rdim) \
  DUNE_STUFF_FUNCTIONS_EXPRESSION_CC_LIST_DOMAINFIELDTYPES(etype, ddim, rdim, 2) \
  DUNE_STUFF_FUNCTIONS_EXPRESSION_CC_LIST_DOMAINFIELDTYPES(etype, ddim, rdim, 3)

#define DUNE_STUFF_FUNCTIONS_EXPRESSION_CC_LIST_DOMAINFIELDTYPES(etype, ddim, rdim, rcdim) \
  DUNE_STUFF_FUNCTIONS_EXPRESSION_CC_LIST_RANGEFIELDTYPES(etype, double, ddim, rdim, rcdim)

#define DUNE_STUFF_FUNCTIONS_EXPRESSION_CC_LIST_RANGEFIELDTYPES(etype, dftype, ddim, rdim, rcdim) \
  DUNE_STUFF_FUNCTIONS_EXPRESSION_CC_LAST_EXPANSION(etype, dftype, ddim, double, rdim, rcdim) \
  DUNE_STUFF_FUNCTIONS_EXPRESSION_CC_LAST_EXPANSION(etype, dftype, ddim, long double, rdim, rcdim)

#define DUNE_STUFF_FUNCTIONS_EXPRESSION_CC_LAST_EXPANSION(etype, dftype, ddim, rftype, rdim, rcdim) \
  template class Dune::Stuff::Function::Expression< etype, dftype, ddim, rftype, rdim, rcdim >;

#ifdef HAVE_DUNE_GRID

# include <dune/grid/sgrid.hh>

typedef typename Dune::SGrid< 1, 1 >::template Codim< 0 >::Entity DuneSGrid1dEntityType;
typedef typename Dune::SGrid< 2, 2 >::template Codim< 0 >::Entity DuneSGrid2dEntityType;
typedef typename Dune::SGrid< 3, 3 >::template Codim< 0 >::Entity DuneSGrid3dEntityType;

DUNE_STUFF_FUNCTIONS_EXPRESSION_CC_LIST_DIMRANGE(DuneSGrid1dEntityType, 1)
DUNE_STUFF_FUNCTIONS_EXPRESSION_CC_LIST_DIMRANGE(DuneSGrid2dEntityType, 2)
DUNE_STUFF_FUNCTIONS_EXPRESSION_CC_LIST_DIMRANGE(DuneSGrid3dEntityType, 3)

# include <dune/grid/yaspgrid.hh>

typedef typename Dune::YaspGrid< 1 >::template Codim< 0 >::Entity DuneYaspGrid1dEntityType;
typedef typename Dune::YaspGrid< 2 >::template Codim< 0 >::Entity DuneYaspGrid2dEntityType;
typedef typename Dune::YaspGrid< 3 >::template Codim< 0 >::Entity DuneYaspGrid3dEntityType;

DUNE_STUFF_FUNCTIONS_EXPRESSION_CC_LIST_DIMRANGE(DuneYaspGrid1dEntityType, 1)
DUNE_STUFF_FUNCTIONS_EXPRESSION_CC_LIST_DIMRANGE(DuneYaspGrid2dEntityType, 2)
DUNE_STUFF_FUNCTIONS_EXPRESSION_CC_LIST_DIMRANGE(DuneYaspGrid3dEntityType, 3)

# if HAVE_ALUGRID_SERIAL_H || HAVE_ALUGRID_PARALLEL_H
#   define ALUGRID_CONFORM 1
#   define ENABLE_ALUGRID 1

#   include <dune/grid/alugrid.hh>

typedef typename Dune::ALUSimplexGrid< 2, 2 >::template Codim< 0 >::Entity DuneAluSimplexGrid2dEntityType;
typedef typename Dune::ALUSimplexGrid< 3, 3 >::template Codim< 0 >::Entity DuneAluSimplexGrid3dEntityType;
typedef typename Dune::ALUCubeGrid< 3, 3 >::template Codim< 0 >::Entity DuneAluCubeGrid3dEntityType;

DUNE_STUFF_FUNCTIONS_EXPRESSION_CC_LIST_DIMRANGE(DuneAluSimplexGrid2dEntityType, 2)
DUNE_STUFF_FUNCTIONS_EXPRESSION_CC_LIST_DIMRANGE(DuneAluSimplexGrid3dEntityType, 3)
DUNE_STUFF_FUNCTIONS_EXPRESSION_CC_LIST_DIMRANGE(DuneAluCubeGrid3dEntityType, 3)

# endif // HAVE_ALUGRID_SERIAL_H || HAVE_ALUGRID_PARALLEL_H
#endif // HAVE_DUNE_GRID

#undef DUNE_STUFF_FUNCTIONS_EXPRESSION_CC_LAST_EXPANSION
#undef DUNE_STUFF_FUNCTIONS_EXPRESSION_CC_LIST_RANGEFIELDTYPES
#undef DUNE_STUFF_FUNCTIONS_EXPRESSION_CC_LIST_DOMAINFIELDTYPES
#undef DUNE_STUFF_FUNCTIONS_EXPRESSION_CC_LIST_DIMRANGECOLS
#undef DUNE_STUFF_FUNCTIONS_EXPRESSION_CC_LIST_DIMRANGE
#endif // DUNE_STUFF_FUNCTIONS_TO_LIB

// This file is part of the dune-stuff project:
//   http://users.dune-project.org/projects/dune-stuff/
// Copyright Holders: Felix Albrecht
// License: BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)

#include "config.h"

#ifdef DUNE_STUFF_FUNCTIONS_TO_LIB
#include "spe10.hh"

#define DUNE_STUFF_FUNCTIONS_SPE10_CC_LIST_DOMAINFIELDTYPES(etype, ddim, rdim, rcdim) \
  DUNE_STUFF_FUNCTIONS_SPE10_CC_LIST_RANGEFIELDTYPES(etype, double, ddim, rdim, rcdim)

#define DUNE_STUFF_FUNCTIONS_SPE10_CC_LIST_RANGEFIELDTYPES(etype, dftype, ddim, rdim, rcdim) \
  DUNE_STUFF_FUNCTIONS_SPE10_CC_LAST_EXPANSION(etype, dftype, ddim, double, rdim, rcdim) \
  DUNE_STUFF_FUNCTIONS_SPE10_CC_LAST_EXPANSION(etype, dftype, ddim, long double, rdim, rcdim)

#define DUNE_STUFF_FUNCTIONS_SPE10_CC_LAST_EXPANSION(etype, dftype, ddim, rftype, rdim, rcdim) \
  template class Dune::Stuff::Function::Spe10Model1< etype, dftype, ddim, rftype, rdim, rcdim >;

#ifdef HAVE_DUNE_GRID

# include <dune/grid/sgrid.hh>

typedef typename Dune::SGrid< 2, 2 >::template Codim< 0 >::Entity DuneSGrid2dEntityType;

DUNE_STUFF_FUNCTIONS_SPE10_CC_LIST_DOMAINFIELDTYPES(DuneSGrid2dEntityType, 2, 1, 1)

# include <dune/grid/yaspgrid.hh>

typedef typename Dune::YaspGrid< 2 >::template Codim< 0 >::Entity DuneYaspGrid2dEntityType;

DUNE_STUFF_FUNCTIONS_SPE10_CC_LIST_DOMAINFIELDTYPES(DuneYaspGrid2dEntityType, 2, 1, 1)

# if HAVE_ALUGRID_SERIAL_H || HAVE_ALUGRID_PARALLEL_H
#   define ALUGRID_CONFORM 1
#   define ENABLE_ALUGRID 1

#   include <dune/grid/alugrid.hh>

typedef typename Dune::ALUSimplexGrid< 2, 2 >::template Codim< 0 >::Entity DuneAluSimplexGrid2dEntityType;

DUNE_STUFF_FUNCTIONS_SPE10_CC_LIST_DOMAINFIELDTYPES(DuneAluSimplexGrid2dEntityType, 2, 1, 1)

# endif // HAVE_ALUGRID_SERIAL_H || HAVE_ALUGRID_PARALLEL_H
#endif // HAVE_DUNE_GRID

#undef DUNE_STUFF_FUNCTIONS_SPE10_CC_LAST_EXPANSION
#undef DUNE_STUFF_FUNCTIONS_SPE10_CC_LIST_RANGEFIELDTYPES
#undef DUNE_STUFF_FUNCTIONS_SPE10_CC_LIST_DOMAINFIELDTYPES
#endif // DUNE_STUFF_FUNCTIONS_TO_LIB

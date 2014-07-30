// This file is part of the dune-stuff project:
//   https://users.dune-project.org/projects/dune-stuff/
// Copyright holders: Rene Milk, Felix Schindler
// License: BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)

#include "config.h"

#if 0 //def DUNE_STUFF_FUNCTIONS_TO_LIB
the whole  DUNE_STUFF_FUNCTIONS_TO_LIB stuff threw loads of errors, so I disabled it everywhere (Rene)

# include "expression.hh"

# define DUNE_STUFF_FUNCTIONS_EXPRESSION_CC_LIST_DIMRANGE(etype, ddim) \
  DUNE_STUFF_FUNCTIONS_EXPRESSION_CC_LIST_DIMRANGECOLS(etype, ddim, 1) \
  DUNE_STUFF_FUNCTIONS_EXPRESSION_CC_LIST_DIMRANGECOLS(etype, ddim, 2) \
  DUNE_STUFF_FUNCTIONS_EXPRESSION_CC_LIST_DIMRANGECOLS(etype, ddim, 3)

# define DUNE_STUFF_FUNCTIONS_EXPRESSION_CC_LIST_DIMRANGECOLS(etype, ddim, rdim) \
  DUNE_STUFF_FUNCTIONS_EXPRESSION_CC_LIST_DOMAINFIELDTYPES(etype, ddim, rdim, 2) \
  DUNE_STUFF_FUNCTIONS_EXPRESSION_CC_LIST_DOMAINFIELDTYPES(etype, ddim, rdim, 3)

# define DUNE_STUFF_FUNCTIONS_EXPRESSION_CC_LIST_DOMAINFIELDTYPES(etype, ddim, rdim, rcdim) \
  DUNE_STUFF_FUNCTIONS_EXPRESSION_CC_LIST_RANGEFIELDTYPES(etype, double, ddim, rdim, rcdim)

# define DUNE_STUFF_FUNCTIONS_EXPRESSION_CC_LIST_RANGEFIELDTYPES(etype, dftype, ddim, rdim, rcdim) \
  DUNE_STUFF_FUNCTIONS_EXPRESSION_CC_LAST_EXPANSION(etype, dftype, ddim, double, rdim, rcdim) \
  DUNE_STUFF_FUNCTIONS_EXPRESSION_CC_LAST_EXPANSION(etype, dftype, ddim, long double, rdim, rcdim)

# define DUNE_STUFF_FUNCTIONS_EXPRESSION_CC_LAST_EXPANSION(etype, dftype, ddim, rftype, rdim, rcdim) \
  template class Dune::Stuff::Functions::Expression< etype, dftype, ddim, rftype, rdim, rcdim >;

# if HAVE_DUNE_GRID

DUNE_STUFF_FUNCTIONS_EXPRESSION_CC_LIST_DIMRANGE(DuneStuffFunctionsInterfacesSGrid1dEntityType, 1)
DUNE_STUFF_FUNCTIONS_EXPRESSION_CC_LIST_DIMRANGE(DuneStuffFunctionsInterfacesSGrid2dEntityType, 2)
DUNE_STUFF_FUNCTIONS_EXPRESSION_CC_LIST_DIMRANGE(DuneStuffFunctionsInterfacesSGrid3dEntityType, 3)

DUNE_STUFF_FUNCTIONS_EXPRESSION_CC_LIST_DIMRANGE(DuneStuffFunctionsInterfacesYaspGrid1dEntityType, 1)
DUNE_STUFF_FUNCTIONS_EXPRESSION_CC_LIST_DIMRANGE(DuneStuffFunctionsInterfacesYaspGrid2dEntityType, 2)
DUNE_STUFF_FUNCTIONS_EXPRESSION_CC_LIST_DIMRANGE(DuneStuffFunctionsInterfacesYaspGrid3dEntityType, 3)

#   if HAVE_ALUGRID_SERIAL_H || HAVE_ALUGRID_PARALLEL_H

DUNE_STUFF_FUNCTIONS_EXPRESSION_CC_LIST_DIMRANGE(DuneStuffFunctionsInterfacesAluSimplexGrid2dEntityType, 2)
DUNE_STUFF_FUNCTIONS_EXPRESSION_CC_LIST_DIMRANGE(DuneStuffFunctionsInterfacesAluSimplexGrid3dEntityType, 3)
DUNE_STUFF_FUNCTIONS_EXPRESSION_CC_LIST_DIMRANGE(DuneStuffFunctionsInterfacesAluCubeGrid3dEntityType, 3)

#   endif // HAVE_ALUGRID_SERIAL_H || HAVE_ALUGRID_PARALLEL_H
# endif // HAVE_DUNE_GRID

# undef DUNE_STUFF_FUNCTIONS_EXPRESSION_CC_LAST_EXPANSION
# undef DUNE_STUFF_FUNCTIONS_EXPRESSION_CC_LIST_RANGEFIELDTYPES
# undef DUNE_STUFF_FUNCTIONS_EXPRESSION_CC_LIST_DOMAINFIELDTYPES
# undef DUNE_STUFF_FUNCTIONS_EXPRESSION_CC_LIST_DIMRANGECOLS
# undef DUNE_STUFF_FUNCTIONS_EXPRESSION_CC_LIST_DIMRANGE
#endif // DUNE_STUFF_FUNCTIONS_TO_LIB

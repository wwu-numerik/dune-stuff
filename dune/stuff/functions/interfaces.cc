// This file is part of the dune-stuff project:
//   https://users.dune-project.org/projects/dune-stuff/
// Copyright holders: Rene Milk, Felix Schindler
// License: BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)

#include "config.h"

#ifdef DUNE_STUFF_FUNCTIONS_TO_LIB
# include "interfaces.hh"

# define DUNE_STUFF_FUNCTIONS_INTERFACES_CC_LIST_CLASSES(etype, ddim) \
  DUNE_STUFF_FUNCTIONS_INTERFACES_CC_LIST_DIMRANGE(Dune::Stuff::LocalfunctionSetInterface, etype, ddim) \
  DUNE_STUFF_FUNCTIONS_INTERFACES_CC_LIST_DIMRANGE(Dune::Stuff::LocalfunctionInterface, etype, ddim) \
  DUNE_STUFF_FUNCTIONS_INTERFACES_CC_LIST_DIMRANGE(Dune::Stuff::LocalizableFunctionInterface, etype, ddim)

# define DUNE_STUFF_FUNCTIONS_INTERFACES_CC_LIST_DIMRANGE(cname, etype, ddim) \
  DUNE_STUFF_FUNCTIONS_INTERFACES_CC_LIST_DIMRANGECOLS(cname, etype, ddim, 1) \
  DUNE_STUFF_FUNCTIONS_INTERFACES_CC_LIST_DIMRANGECOLS(cname, etype, ddim, 2) \
  DUNE_STUFF_FUNCTIONS_INTERFACES_CC_LIST_DIMRANGECOLS(cname, etype, ddim, 3)

# define DUNE_STUFF_FUNCTIONS_INTERFACES_CC_LIST_DIMRANGECOLS(cname, etype, ddim, rdim) \
  DUNE_STUFF_FUNCTIONS_INTERFACES_CC_LIST_DOMAINFIELDTYPES(cname, etype, ddim, rdim, 1) \
  DUNE_STUFF_FUNCTIONS_INTERFACES_CC_LIST_DOMAINFIELDTYPES(cname, etype, ddim, rdim, 2) \
  DUNE_STUFF_FUNCTIONS_INTERFACES_CC_LIST_DOMAINFIELDTYPES(cname, etype, ddim, rdim, 3)

# define DUNE_STUFF_FUNCTIONS_INTERFACES_CC_LIST_DOMAINFIELDTYPES(cname, etype, ddim, rdim, rcdim) \
  DUNE_STUFF_FUNCTIONS_INTERFACES_CC_LIST_RANGEFIELDTYPES(cname, etype, double, ddim, rdim, rcdim)

# define DUNE_STUFF_FUNCTIONS_INTERFACES_CC_LIST_RANGEFIELDTYPES(cname, etype, dftype, ddim, rdim, rcdim) \
  DUNE_STUFF_FUNCTIONS_INTERFACES_CC_LAST_EXPANSION(cname, etype, dftype, ddim, double, rdim, rcdim) \
  DUNE_STUFF_FUNCTIONS_INTERFACES_CC_LAST_EXPANSION(cname, etype, dftype, ddim, long double, rdim, rcdim)

# define DUNE_STUFF_FUNCTIONS_INTERFACES_CC_LAST_EXPANSION(cname, etype, dftype, ddim, rftype, rdim, rcdim) \
  template class cname< etype, dftype, ddim, rftype, rdim, rcdim >;

DUNE_STUFF_FUNCTIONS_INTERFACES_CC_LIST_CLASSES(DuneStuffFunctionsInterfacesFake1dEntityType, 1)
DUNE_STUFF_FUNCTIONS_INTERFACES_CC_LIST_CLASSES(DuneStuffFunctionsInterfacesFake2dEntityType, 2)
DUNE_STUFF_FUNCTIONS_INTERFACES_CC_LIST_CLASSES(DuneStuffFunctionsInterfacesFake3dEntityType, 3)

# if HAVE_DUNE_GRID

DUNE_STUFF_FUNCTIONS_INTERFACES_CC_LIST_CLASSES(DuneStuffFunctionsInterfacesSGrid1dEntityType, 1)
DUNE_STUFF_FUNCTIONS_INTERFACES_CC_LIST_CLASSES(DuneStuffFunctionsInterfacesSGrid2dEntityType, 2)
DUNE_STUFF_FUNCTIONS_INTERFACES_CC_LIST_CLASSES(DuneStuffFunctionsInterfacesSGrid3dEntityType, 3)

DUNE_STUFF_FUNCTIONS_INTERFACES_CC_LIST_CLASSES(DuneStuffFunctionsInterfacesYaspGrid1dEntityType, 1)
DUNE_STUFF_FUNCTIONS_INTERFACES_CC_LIST_CLASSES(DuneStuffFunctionsInterfacesYaspGrid2dEntityType, 2)
DUNE_STUFF_FUNCTIONS_INTERFACES_CC_LIST_CLASSES(DuneStuffFunctionsInterfacesYaspGrid3dEntityType, 3)

#   if HAVE_ALUGRID_SERIAL_H || HAVE_ALUGRID_PARALLEL_H

DUNE_STUFF_FUNCTIONS_INTERFACES_CC_LIST_CLASSES(DuneStuffFunctionsInterfacesAluSimplexGrid2dEntityType, 2)
DUNE_STUFF_FUNCTIONS_INTERFACES_CC_LIST_CLASSES(DuneStuffFunctionsInterfacesAluSimplexGrid3dEntityType, 3)
DUNE_STUFF_FUNCTIONS_INTERFACES_CC_LIST_CLASSES(DuneStuffFunctionsInterfacesAluCubeGrid3dEntityType, 3)

#   endif // HAVE_ALUGRID_SERIAL_H || HAVE_ALUGRID_PARALLEL_H
# endif // HAVE_DUNE_GRID

# undef DUNE_STUFF_FUNCTIONS_INTERFACES_CC_LAST_EXPANSION
# undef DUNE_STUFF_FUNCTIONS_INTERFACES_CC_LIST_RANGEFIELDTYPES
# undef DUNE_STUFF_FUNCTIONS_INTERFACES_CC_LIST_DOMAINFIELDTYPES
# undef DUNE_STUFF_FUNCTIONS_INTERFACES_CC_LIST_DIMRANGECOLS
# undef DUNE_STUFF_FUNCTIONS_INTERFACES_CC_LIST_DIMRANGE
# undef DUNE_STUFF_FUNCTIONS_INTERFACES_CC_LIST_CLASSES

template class Dune::Stuff::FunctionInterface< double, 1, double, 1 >;
template class Dune::Stuff::FunctionInterface< double, 1, double, 2 >;
template class Dune::Stuff::FunctionInterface< double, 1, double, 3 >;

template class Dune::Stuff::FunctionInterface< double, 2, double, 1 >;
template class Dune::Stuff::FunctionInterface< double, 2, double, 2 >;
template class Dune::Stuff::FunctionInterface< double, 2, double, 3 >;

template class Dune::Stuff::FunctionInterface< double, 3, double, 1 >;
template class Dune::Stuff::FunctionInterface< double, 3, double, 2 >;
template class Dune::Stuff::FunctionInterface< double, 3, double, 3 >;

#endif // DUNE_STUFF_FUNCTIONS_TO_LIB

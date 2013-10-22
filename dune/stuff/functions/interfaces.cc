// This file is part of the dune-stuff project:
//   http://users.dune-project.org/projects/dune-stuff/
// Copyright Holders: Felix Albrecht, Rene Milk
// License: BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)

#include "config.h"

#include "interfaces.hh"

#define DUNE_STUFF_FUNCTION_INTERFACE_CC_LIST_CLASSES(etype, ddim) \
  DUNE_STUFF_FUNCTION_INTERFACE_CC_LIST_DIMRANGE(Dune::Stuff::LocalfunctionSetInterface, etype, ddim) \
  DUNE_STUFF_FUNCTION_INTERFACE_CC_LIST_DIMRANGE(Dune::Stuff::LocalfunctionInterface, etype, ddim) \
  DUNE_STUFF_FUNCTION_INTERFACE_CC_LIST_DIMRANGE(Dune::Stuff::LocalizableFunctionInterface, etype, ddim)

#define DUNE_STUFF_FUNCTION_INTERFACE_CC_LIST_DIMRANGE(cname, etype, ddim) \
  DUNE_STUFF_FUNCTION_INTERFACE_CC_LIST_DIMRANGECOLS(cname, etype, ddim, 1) \
  DUNE_STUFF_FUNCTION_INTERFACE_CC_LIST_DIMRANGECOLS(cname, etype, ddim, 2) \
  DUNE_STUFF_FUNCTION_INTERFACE_CC_LIST_DIMRANGECOLS(cname, etype, ddim, 3)

#define DUNE_STUFF_FUNCTION_INTERFACE_CC_LIST_DIMRANGECOLS(cname, etype, ddim, rdim) \
  DUNE_STUFF_FUNCTION_INTERFACE_CC_LIST_DOMAINFIELDTYPES(cname, etype, ddim, rdim, 1) \
  DUNE_STUFF_FUNCTION_INTERFACE_CC_LIST_DOMAINFIELDTYPES(cname, etype, ddim, rdim, 2) \
  DUNE_STUFF_FUNCTION_INTERFACE_CC_LIST_DOMAINFIELDTYPES(cname, etype, ddim, rdim, 3)

#define DUNE_STUFF_FUNCTION_INTERFACE_CC_LIST_DOMAINFIELDTYPES(cname, etype, ddim, rdim, rcdim) \
  DUNE_STUFF_FUNCTION_INTERFACE_CC_LIST_RANGEFIELDTYPES(cname, etype, double, ddim, rdim, rcdim)

#define DUNE_STUFF_FUNCTION_INTERFACE_CC_LIST_RANGEFIELDTYPES(cname, etype, dftype, ddim, rdim, rcdim) \
  DUNE_STUFF_FUNCTION_INTERFACE_CC_LAST_EXPANSION(cname, etype, dftype, ddim, double, rdim, rcdim) \
  DUNE_STUFF_FUNCTION_INTERFACE_CC_LAST_EXPANSION(cname, etype, dftype, ddim, long double, rdim, rcdim)

#define DUNE_STUFF_FUNCTION_INTERFACE_CC_LAST_EXPANSION(cname, etype, dftype, ddim, rftype, rdim, rcdim) \
  template class cname< etype, dftype, ddim, rftype, rdim, rcdim >;

#include <dune/stuff/grid/fakeentity.hh>

typedef Dune::Stuff::Grid::FakeEntity< 1 > DuneStuffFake1dEntityType;
typedef Dune::Stuff::Grid::FakeEntity< 2 > DuneStuffFake2dEntityType;
typedef Dune::Stuff::Grid::FakeEntity< 3 > DuneStuffFake3dEntityType;

DUNE_STUFF_FUNCTION_INTERFACE_CC_LIST_CLASSES(DuneStuffFake1dEntityType, 1)
DUNE_STUFF_FUNCTION_INTERFACE_CC_LIST_CLASSES(DuneStuffFake2dEntityType, 2)
DUNE_STUFF_FUNCTION_INTERFACE_CC_LIST_CLASSES(DuneStuffFake3dEntityType, 3)

#ifdef HAVE_DUNE_GRID

# include <dune/grid/sgrid.hh>

typedef typename Dune::SGrid< 1, 1 >::template Codim< 0 >::Entity DuneSGrid1dEntityType;
typedef typename Dune::SGrid< 2, 2 >::template Codim< 0 >::Entity DuneSGrid2dEntityType;
typedef typename Dune::SGrid< 3, 3 >::template Codim< 0 >::Entity DuneSGrid3dEntityType;

DUNE_STUFF_FUNCTION_INTERFACE_CC_LIST_CLASSES(DuneSGrid1dEntityType, 1)
DUNE_STUFF_FUNCTION_INTERFACE_CC_LIST_CLASSES(DuneSGrid2dEntityType, 2)
DUNE_STUFF_FUNCTION_INTERFACE_CC_LIST_CLASSES(DuneSGrid3dEntityType, 3)

# include <dune/grid/yaspgrid.hh>

typedef typename Dune::YaspGrid< 1 >::template Codim< 0 >::Entity DuneYaspGrid1dEntityType;
typedef typename Dune::YaspGrid< 2 >::template Codim< 0 >::Entity DuneYaspGrid2dEntityType;
typedef typename Dune::YaspGrid< 3 >::template Codim< 0 >::Entity DuneYaspGrid3dEntityType;

DUNE_STUFF_FUNCTION_INTERFACE_CC_LIST_CLASSES(DuneYaspGrid1dEntityType, 1)
DUNE_STUFF_FUNCTION_INTERFACE_CC_LIST_CLASSES(DuneYaspGrid2dEntityType, 2)
DUNE_STUFF_FUNCTION_INTERFACE_CC_LIST_CLASSES(DuneYaspGrid3dEntityType, 3)

# if HAVE_ALUGRID_SERIAL_H || HAVE_ALUGRID_PARALLEL_H
#   define ALUGRID_CONFORM 1
#   define ENABLE_ALUGRID 1

#   include <dune/grid/alugrid.hh>

typedef typename Dune::ALUSimplexGrid< 2, 2 >::template Codim< 0 >::Entity DuneAluSimplexGrid2dEntityType;
typedef typename Dune::ALUSimplexGrid< 3, 3 >::template Codim< 0 >::Entity DuneAluSimplexGrid3dEntityType;
typedef typename Dune::ALUCubeGrid< 3, 3 >::template Codim< 0 >::Entity DuneAluCubeGrid3dEntityType;

DUNE_STUFF_FUNCTION_INTERFACE_CC_LIST_CLASSES(DuneAluSimplexGrid2dEntityType, 2)
DUNE_STUFF_FUNCTION_INTERFACE_CC_LIST_CLASSES(DuneAluSimplexGrid3dEntityType, 3)
DUNE_STUFF_FUNCTION_INTERFACE_CC_LIST_CLASSES(DuneAluCubeGrid3dEntityType, 3)

# endif // HAVE_ALUGRID_SERIAL_H || HAVE_ALUGRID_PARALLEL_H
#endif // HAVE_DUNE_GRID

template class Dune::Stuff::FunctionInterface< double, 1, double, 1 >;
template class Dune::Stuff::FunctionInterface< double, 1, double, 2 >;
template class Dune::Stuff::FunctionInterface< double, 1, double, 3 >;

template class Dune::Stuff::FunctionInterface< double, 2, double, 1 >;
template class Dune::Stuff::FunctionInterface< double, 2, double, 2 >;
template class Dune::Stuff::FunctionInterface< double, 2, double, 3 >;

template class Dune::Stuff::FunctionInterface< double, 3, double, 1 >;
template class Dune::Stuff::FunctionInterface< double, 3, double, 2 >;
template class Dune::Stuff::FunctionInterface< double, 3, double, 3 >;

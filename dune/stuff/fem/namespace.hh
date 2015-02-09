// This file is part of the dune-stuff project:
//   https://github.com/wwu-numerik/dune-stuff
// Copyright holders: Rene Milk, Felix Schindler
// License: BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)

#warning Include dune/stuff/fem.hh instead (09.02.2015)!

#include <dune/stuff/fem.hh>

#ifndef DUNE_STUFF_FEM_NAMESPACE_HH
#define DUNE_STUFF_FEM_NAMESPACE_HH

#include <dune/common/version.hh>

#if HAVE_DUNE_FEM
# include <dune/fem/version.hh>
#endif


#if HAVE_DUNE_FEM
# if DUNE_VERSION_EQUAL_REV(DUNE_FEM,1,4,-1)
#   define DUNE_FEM_IS_LOCALFUNCTIONS_COMPATIBLE 1
# elif DUNE_VERSION_EQUAL_REV(DUNE_FEM,1,4,-2)
#   define DUNE_FEM_IS_LOCALFUNCTIONS_COMPATIBLE 0
# else
#   define DUNE_FEM_IS_LOCALFUNCTIONS_COMPATIBLE 0
# endif
#else
# define DUNE_FEM_IS_LOCALFUNCTIONS_COMPATIBLE 0
#endif // HAVE_DUNE_FEM

#endif // DUNE_STUFF_FEM_NAMESPACE_HH

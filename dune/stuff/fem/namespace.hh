#ifndef DUNE_STUFF_FEM_NAMESPACE_HH
#define DUNE_STUFF_FEM_NAMESPACE_HH

#ifdef HAVE_CMAKE_CONFIG
  #include "cmake_config.h"
#elif defined (HAVE_CONFIG_H)
  #include "config.h"
#endif

#include <dune/common/version.hh>

#ifdef HAVE_DUNE_FEM

#if DUNE_VERSION_EQUAL_REV(DUNE_FEM,1,4,-1)
  #define DUNE_FEM_IS_LOCALFUNCTIONS_COMPATIBLE
#else
  #define DUNE_FEM_IS_UNKNOWN
#endif

#endif // HAVE_DUNE_FEM

#endif // DUNE_STUFF_FEM_NAMESPACE_HH

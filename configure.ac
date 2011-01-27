# -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.
DUNE_AC_INIT # gets module version from dune.module file
AM_INIT_AUTOMAKE
AC_CONFIG_SRCDIR([stuff/printing.hh])
AM_CONFIG_HEADER([config.h])


# we need no more than the standard DE-stuff
# this module depends on dune-common dune-grid dune-fem
# this implies checking for [dune-common], [dune-grid], [dune-istl], [dune-fem]
DUNE_CHECK_ALL
DUNE_SYMLINK

# implicitly set the Dune-flags everywhere
AC_SUBST(DUNE_STUFF_ROOT, '$(top_buildir)')
AC_SUBST(DUNE_STUFF_CPPFLAGS, '-I$(top_buildir)')
AC_SUBST(AM_CPPFLAGS, '$(DUNE_CPPFLAGS) -I$(top_srcdir)')
AC_SUBST(AM_LDFLAGS, '$(DUNE_LDFLAGS) $(DUNE_LIBS)')
AC_SUBST([LOCAL_LIBS], '')



AC_CONFIG_FILES([Makefile 
	stuff/Makefile
	dune-stuff.pc])
AC_OUTPUT
# finally print the summary information
DUNE_SUMMARY_ALL

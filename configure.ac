#! /bin/bash

# This file is part of the dune-stuff project:
#   http://users.dune-project.org/projects/dune-stuff
# Copyright holders: Rene Milk, Felix Schindler
# License: BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)
#
# Contributors: Sven Kaulmann

# Process this file with autoconf to produce a configure script.
AC_PREREQ(2.50)
DUNE_AC_INIT # gets module version from dune.module file

AC_CONFIG_SRCDIR([dune/stuff/aliases.hh])
AM_CONFIG_HEADER([config.h])

AM_INIT_AUTOMAKE([subdir-objects])

# check all dune-module stuff
DUNE_CHECK_ALL


# preset variable to path such that #include <dune/...> works
AC_SUBST([DUNE_STUFF_ROOT], '$(top_builddir)')
AC_SUBST([AM_CPPFLAGS], '$(DUNE_CPPFLAGS) -I$(top_srcdir)')
AC_SUBST([AM_LDFLAGS], '$(DUNE_LDFLAGS) $(DUNE_LIBS)')
AC_SUBST([LOCAL_LIBS], '')

AC_DEFINE([HAVE_DUNE_STUFF], [1], [We are always here.])

AC_CONFIG_FILES([Makefile
	lib/Makefile
	dune/Makefile
	dune/stuff/Makefile
	dune/stuff/common/Makefile
	dune/stuff/fem/Makefile
	dune/stuff/fem/functions/Makefile
	dune/stuff/grid/Makefile
	dune/stuff/grid/provider/Makefile
	dune/stuff/grid/output/Makefile
	dune/stuff/discretefunction/Makefile
	dune/stuff/discretefunction/projection/Makefile
	dune/stuff/test/Makefile
	examples/Makefile
	examples/grid/Makefile
	m4/Makefile
	dune-stuff.pc])


AC_OUTPUT
# finally print the summary information
DUNE_SUMMARY_ALL

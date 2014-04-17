# This file is part of the dune-stuff project:
#   http://users.dune-project.org/projects/dune-stuff
# Copyright holders: Rene Milk, Felix Schindler
# License: BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)
#
# Contributors: Sven Kaulmann

AC_DEFUN([DUNE_STUFF_CHECKS],[
  AC_DEFINE([DUNE_STUFF_SSIZE_T], [long int], [We need this for the python bindings (there is no unsigned in python).])

  AX_BOOST_BASE([1.48.0],
                [AC_DEFINE([HAVE_BOOST],
                           [],
                           [Define wether boost was found.])],
                           [AC_MSG_ERROR([Boost 1.48 is required])])
  AX_BOOST_FILESYSTEM([1.48.0])
  AX_BOOST_SYSTEM([1.48.0])
  AX_BOOST_TIMER([1.48.0])
  AX_BOOST_CHRONO([1.48.0])

  AC_LANG([C++])
  AX_CXX_COMPILE_STDCXX_11([noext],[mandatory])

  AX_OVERRIDE_KEYWORD_CHECK()
  AX_FINAL_KEYWORD_CHECK()

  PKG_CHECK_MODULES([EIGEN],
                    [eigen3],
                    [AC_DEFINE([HAVE_EIGEN],
                               [1],
                               [Define wether the eigen includes were found.])],
                               [AC_MSG_RESULT([Package 'eigen3' was not found in the pkg-config search path.
Perhaps you should add the directory containing `eigen3.pc'
to the PKG_CONFIG_PATH environment variable.])])
  DUNE_CPPFLAGS="$DUNE_CPPFLAGS $EIGEN_CFLAGS"

  PKG_CHECK_MODULES([LIBAMA],
                    [libama],
                    [AC_DEFINE([HAVE_LIBAMA],
                               [1],
                               [Define wether libama.])],
                               [AC_MSG_RESULT([Package 'libama' was not found in the pkg-config search path.
Perhaps you should add the directory containing `libama.pc'
to the PKG_CONFIG_PATH environment variable.])])
  DUNE_CPPFLAGS="$DUNE_CPPFLAGS $LIBAMA_CFLAGS"
  DUNE_LDFLAGS="$DUNE_LDFLAGS $LIBAMA_LDFLAGS"
])

AC_DEFUN([DUNE_STUFF_CHECK_MODULE],
[
    DUNE_CHECK_MODULES([dune-stuff], [stuff/common/logging.hh], [Dune::Stuff::Common::Logger()])
])

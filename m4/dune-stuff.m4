AC_DEFUN([DUNE_STUFF_CHECKS],[
  AX_BOOST_BASE([1.48.0],
                [AC_DEFINE([HAVE_BOOST],
                           [],
                           [Define wether boost was found.])],
                           [AC_MSG_ERROR([Boost 1.48 is required])])
  AX_BOOST_FILESYSTEM([1.48.0])
  AC_LANG([C++])
  AX_CXX_COMPILE_STDCXX_11([noext],[mandatory])
  PKG_CHECK_MODULES([EIGEN],
                    [eigen3],
                    [AC_DEFINE([HAVE_EIGEN],
                               [1],
                               [Define wether the eigen includes were found.])],
                               [AC_MSG_RESULT([Package 'eigen3' was not found in the pkg-config search path.
Perhaps you should add the directory containing `eigen3.pc'
to the PKG_CONFIG_PATH environment variable.])])
  DUNE_CPPFLAGS="$DUNE_CPPFLAGS $EIGEN_CFLAGS"
])

AC_DEFUN([DUNE_STUFF_CHECK_MODULE],
[
    DUNE_CHECK_MODULES([dune-stuff], [stuff/common/profiler.hh], [Dune::Stuff::Common::profiler()])
])

AC_DEFUN([DUNE_STUFF_CHECKS],[
  AX_BOOST_BASE([1.48.0],
                [AC_DEFINE([HAVE_BOOST],
                           [],
                           [Define wether boost was found.])],
                           [AC_MSG_ERROR([Boost 1.48 is required])])
  AX_BOOST_FILESYSTEM([1.48.0])
  AC_REQUIRE([GXX0X])
  AC_REQUIRE([LAMBDAS_CHECK])
  AC_REQUIRE([STD_BEGIN_END_CHECK])
  AC_REQUIRE([CONSTEXPR_CHECK])
  PKG_CHECK_MODULES([EIGEN],
                    [eigen3],
                    [AC_DEFINE([HAVE_EIGEN],
                               [1],
                               [Define wether the eigen includes were found.])],
                               [AC_MSG_RESULT([No package 'eigen3' found

Consider adjusting the PKG_CONFIG_PATH environment variable if you
installed 'eigen3' in a non-standard prefix.])])
])

AC_DEFUN([DUNE_STUFF_CHECK_MODULE],
[
    DUNE_CHECK_MODULES([dune-stuff], [stuff/exists.hh])
])

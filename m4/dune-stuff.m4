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
])

AC_DEFUN([DUNE_STUFF_CHECK_MODULE],
[
    DUNE_CHECK_MODULES([dune-stuff], [stuff/exists.hh])
])

AC_DEFUN([LAMBDAS_CHECK],[
  AC_CACHE_CHECK([whether lambdas are supported], dune_cv_lambdas_support, [
    AC_REQUIRE([AC_PROG_CXX])
    AC_REQUIRE([GXX0X])
    AC_LANG_PUSH([C++])
    AC_TRY_COMPILE([],[auto f = [] (){ return 0; };],
      dune_cv_lambdas_support=yes,
      dune_cv_lambdas_support=no)
    AC_LANG_POP
  ])
  if test "x$dune_cv_lambdas_support" = xyes; then
    AC_DEFINE(HAS_LAMBDA_FUNCTIONS, 1, [Define to 1 if lambdas are supported])
  fi
])
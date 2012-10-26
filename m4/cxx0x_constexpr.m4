AC_DEFUN([CONSTEXPR_CHECK],[
  AC_CACHE_CHECK([whether constexpr are supported], dune_cv_constexprs_support, [
    AC_REQUIRE([AC_PROG_CXX])
    AC_REQUIRE([GXX0X])
    AC_LANG_PUSH([C++])
    AC_TRY_COMPILE([],[constexpr int f = 1;],
      dune_cv_constexprs_support=yes,
      dune_cv_constexprs_support=no)
    AC_LANG_POP
  ])
  if test "x$dune_cv_constexprs_support" = xyes; then
    AC_DEFINE(HAS_CONSTEXPR, 1, [Define to 1 if constexpr are supported])
  fi
])
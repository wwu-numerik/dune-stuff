AC_DEFUN([STD_BEGIN_END_CHECK],[
  AC_CACHE_CHECK([whether std::begin/end are supported], dune_cv_begin_end_support, [
    AC_REQUIRE([AC_PROG_CXX])
    AC_REQUIRE([GXX0X])
    AC_LANG_PUSH([C++])
    AC_TRY_COMPILE([
				#include <vector>
				#include <iterator>
				],[
				std::vector<int> a;
				std::vector<int>::const_iterator b = std::begin(a);
				std::vector<int>::const_iterator e = std::end(a);
				],
      dune_cv_begin_end_support=yes,
      dune_cv_begin_end_support=no)
    AC_LANG_POP
  ])
  if test "x$dune_cv_begin_end_support" = xyes; then
    AC_DEFINE(HAS_STD_BEGIN_END, 1, [Define to 1 if std::begin/end are supported])
  fi
])
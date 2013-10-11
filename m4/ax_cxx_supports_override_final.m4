AC_DEFUN([AX_OVERRIDE_KEYWORD_CHECK],[
  AC_CACHE_CHECK([whether $CXX supports the override keyword], dune_stuff_override_support, [
    AC_REQUIRE([AC_PROG_CXX])
    AC_REQUIRE([GXX0X])
    AC_LANG_PUSH([C++])
    AC_COMPILE_IFELSE([AC_LANG_PROGRAM(
      [],[[
      struct IFoo
      {
        virtual ~IFoo() {}
        virtual void bar() = 0;
      };

      struct Foo
        : public IFoo
      {
        virtual void bar() override {}
      };

      IFoo* foo = new Foo;
      delete foo;
      ]])],
      dune_stuff_override_support=yes,
      dune_stuff_override_support=no)
    AC_LANG_POP
  ])
  if test "x$dune_stuff_override_support" = xyes; then
    AC_DEFINE(DS_OVERRIDE, override, [Define override keyword, if supported])
  else
    AC_DEFINE(DS_OVERRIDE, /*override*/, [Define override keyword, if supported])
  fi
])

AC_DEFUN([AX_FINAL_KEYWORD_CHECK],[
  AC_CACHE_CHECK([whether $CXX supports the final keyword], dune_stuff_final_support, [
    AC_REQUIRE([AC_PROG_CXX])
    AC_REQUIRE([GXX0X])
    AC_LANG_PUSH([C++])
    AC_COMPILE_IFELSE([AC_LANG_PROGRAM(
      [],[[
      struct IFoo
      {
        virtual ~IFoo() {}
        virtual void bar() = 0;
      };

      struct Foo
        : public IFoo
      {
        virtual void bar() final {}
      };

      IFoo* foo = new Foo;
      delete foo;
      ]])],
      dune_stuff_final_support=yes,
      dune_stuff_final_support=no)
    AC_LANG_POP
  ])
  if test "x$dune_stuff_final_support" = xyes; then
    AC_DEFINE(DS_FINAL, final, [Define final keyword, if supported])
  else
    AC_DEFINE(DS_FINAL, /*final*/, [Define final keyword, if supported])
  fi
])

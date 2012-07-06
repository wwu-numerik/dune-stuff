AC_DEFUN([DUNE_STUFF_CHECKS],[
  AX_BOOST_BASE([1.41.0])
])

AC_DEFUN([DUNE_STUFF_CHECK_MODULE],
[
    DUNE_CHECK_MODULES([dune-stuff], [stuff/common/debug.hh])
])

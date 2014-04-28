# This file is part of the dune-stuff project:
#   http://users.dune-project.org/projects/dune-stuff
# Copyright holders: Rene Milk, Felix Schindler
# License: BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)

dunemodulelibdir=$(libdir)


#the dune-stuff library
lib_LTLIBRARIES = libdunestuff.la libdunestuffpythonbindings.la

# this is just a renaming of libstuff to prevent name clashes
libdunestuff_la_SOURCES =
# This forces automake to use the C++ linker
# (see the automake manual, section "Libtool Convenience Libraries")
nodist_EXTRA_libdunestuff_la_SOURCES = dummy.cc
sourcescheck_DUMMY = dummy.cc
libdunestuff_la_LIBADD = ../dune/stuff/libstuff.la \
  $(BOOST_FILESYSTEM_LIB) $(BOOST_SYSTEM_LIB) \
  $(BOOST_TIMER_LIB) $(BOOST_CHRONO_LIB)

# this is just a renaming of libstuff to prevent name clashes
libdunestuffpythonbindings_la_SOURCES =
# This forces automake to use the C++ linker
# (see the automake manual, section "Libtool Convenience Libraries")
nodist_EXTRA_libdunestuffpythonbindings_la_SOURCES = dummy.cc
libdunestuffpythonbindings_la_LIBADD = ../dune/stuff/libstuffpythonbindings.la \
  $(BOOST_FILESYSTEM_LIB) $(BOOST_SYSTEM_LIB) \
  $(BOOST_TIMER_LIB) $(BOOST_CHRONO_LIB)

include $(top_srcdir)/am/global-rules

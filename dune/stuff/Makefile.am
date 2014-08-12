# This file is part of the dune-stuff project:
#   https://github.com/wwu-numerik/dune-stuff
# Copyright holders: Rene Milk, Felix Schindler
# License: BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)

stuffdir = $(includedir)/dune/stuff

SUBDIRS = . test

noinst_LTLIBRARIES = libstuff.la

libstuff_la_SOURCES = \
	common/filesystem.cc \
	common/color.cc \
	common/logging.cc \
	common/logstreams.cc \
	common/math.cc \
	common/profiler.cc \
	common/configcontainer.cc \
	common/signals.cc \
	common/parallel/threadmanager.cc \
	fem/femeoc.cc \
	grid/fakeentity.cc \
	functions/interfaces.cc \
	functions/checkerboard.cc \
	functions/constant.cc \
	functions/expression/mathexpr.cc \
	functions/expression.cc \
	functions/spe10.cc \
	functions.cc \
	la/container/pattern.cc

libstuff_la_LIBADD = common $(BOOST_FILESYSTEM_LIB) $(BOOST_SYSTEM_LIB) \
	$(BOOST_TIMER_LIB) $(BOOST_CHRONO_LIB) $(ALUGRID_LIBS)

libstuff_la_CPPFLAGS = $(DUNE_CPPFLAGS) $(ALUGRID_CPPFLAGS)

include $(top_srcdir)/am/global-rules

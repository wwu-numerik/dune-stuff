stuffdir = $(includedir)/dune/stuff

SUBDIRS = . common discretefunction fem grid test

noinst_LTLIBRARIES = libstuff.la

libstuff_la_SOURCES = \
	common/filesystem.cc \
	common/color.cc \
	common/logging.cc \
	common/logstreams.cc \
	common/math.cc \
	common/profiler.cc \
	common/parameter/configcontainer.cc \
	common/parameter/tree.cc \
	common/signals.cc \
	fem/femeoc.cc \
  grid/fakeentity.cc \
	functions/expression/mathexpr.cc \
	functions/interfaces.cc \
	functions/constant.cc \
  la/container/pattern.cc

libstuff_la_LIBADD = common $(BOOST_FILESYSTEM_LIB) $(BOOST_SYSTEM_LIB) \
	$(BOOST_TIMER_LIB) $(BOOST_CHRONO_LIB) $(ALUGRID_LIBS)

libstuff_la_CPPFLAGS = $(DUNE_CPPFLAGS) $(ALUGRID_CPPFLAGS)

include $(top_srcdir)/am/global-rules

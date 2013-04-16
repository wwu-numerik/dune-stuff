stuffdir = $(includedir)/dune/stuff

SUBDIRS = . common discretefunction fem function grid test 

noinst_LTLIBRARIES = libstuff.la

libstuff_la_SOURCES = \
	common/filesystem.cc \
	common/color.cc \
	common/logging.cc \
	common/logstreams.cc \
	common/profiler.cc \
	common/parameter/configcontainer.cc \
	common/parameter/tree.cc \
	common/signals.cc \
	fem/femeoc.cc 
	
libstuff_la_LIBADD = common $(BOOST_FILESYSTEM_LIB) $(BOOST_SYSTEM_LIB) \
	$(BOOST_TIMER_LIB) $(BOOST_CHRONO_LIB) 
	
include $(top_srcdir)/am/global-rules

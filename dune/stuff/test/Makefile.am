# $Id$

GRIDTYPE=YASPGRID
GRIDDIM=2

# tests where program to build and program to run are equal
NORMALTESTS = common_parameter_validation common_logger common_color function_expression_2

# list of tests to run
TESTS = $(NORMALTESTS)

# programs just to build when "make check" is used
check_PROGRAMS = $(NORMALTESTS)

#
## common flags
#

# output coverage
COVERAGE = -fprofile-arcs -ftest-coverage
AM_CXXFLAGS = $(COVERAGE)

common_LDADD = -lboost_filesystem -lboost_system $(LDADD)
common_CPPFLAGS = $(AM_CPPFLAGS) $(BOOST_CPPFLAGS) $(DUNE_CPPFLAGS) \
	-DGRIDDIM=$(GRIDDIM) -D$(GRIDTYPE)
#
## define the programs
#

common_parameter_validation_SOURCES = common_parameter_validation.cc

common_logger_SOURCES = common_logger.cc
common_logger_CPPFLAGS = $(common_CPPFLAGS)
common_logger_LDADD = $(common_LDADD)

common_color_SOURCES = common_color.cc

function_expression_2_SOURCES = function_expression_2.cc
function_expression_2_LDADD = $(common_LDADD)
function_expression_2_CPPFLAGS = $(common_CPPFLAGS) 

## distribution tarball
SOURCES = $(common_parameter_validation_SOURCES) $(common_logger_SOURCES)

# gridcheck not used explicitly, we should still ship it :)
EXTRA_DIST = $(SOURCES)

CLEANFILES = *.gcda *.gcno semantic.cache simplex-testgrid*.dgf.* cube-testgrid*.dgf.* dgfparser.log

include $(top_srcdir)/am/global-rules

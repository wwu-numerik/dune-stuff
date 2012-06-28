# $Id$

# tests where program to build and program to run are equal
NORMALTESTS = test_validation

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


#
## define the programs
#

test_validation_SOURCES = test_validation.cc

## distribution tarball
SOURCES = $(test_validation_SOURCES)

# gridcheck not used explicitly, we should still ship it :)
EXTRA_DIST = $(SOURCES)

CLEANFILES = *.gcda *.gcno semantic.cache simplex-testgrid*.dgf.* cube-testgrid*.dgf.* dgfparser.log

include $(top_srcdir)/am/global-rules

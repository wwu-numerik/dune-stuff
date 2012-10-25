SUBDIRS = 

GRIDTYPE=YASPGRID
GRIDDIM=2

# tests where program to build and program to run are equal
NORMALTESTS = common_parameter_validation common_logger common_color \
	common_math common_profiler common_string common_typenames \
	function_expression 

GTEST_SRC = gtest/gtest-all.cc

LDFLAGS = $(BOOST_LDFLAGS) -lboost_timer -lboost_chrono -lboost_filesystem -lboost_system
CXXFLAGS = $(BOOST_CPPFLAGS) $(DUNE_CPPFLAGS) \
	-DGRIDDIM=$(GRIDDIM) -D$(GRIDTYPE) -I.

common_parameter_validation_SOURCES = common_parameter_validation.cc ${GTEST_SRC}
common_logger_SOURCES = common_logger.cc ${GTEST_SRC}
common_math_SOURCES = common_math.cc ${GTEST_SRC}
common_profiler_SOURCES = common_profiler.cc ${GTEST_SRC}
common_string_SOURCES = common_string.cc ${GTEST_SRC}
common_typenames_SOURCES = common_typenames.cc ${GTEST_SRC}
common_color_SOURCES = common_color.cc ${GTEST_SRC}

function_expression_SOURCES = function_expression.cc ${GTEST_SRC}

SOURCES = $(common_parameter_validation_SOURCES) $(common_logger_SOURCES)
	$(common_math_SOURCES) $(common_profiler_SOURCES) $(common_string_SOURCES) \
	$(common_typenames_SOURCES) $(common_color_SOURCES) 

if HAVE_DUNE_GRID
	grid_provider_SOURCES = grid_provider.cc ${GTEST_SRC}
	grid_information_SOURCES = grid_information.cc ${GTEST_SRC}
	grid_output_pgf_SOURCES = grid_output_pgf.cc ${GTEST_SRC}
	NORMALTESTS += grid_provider grid_information
endif
SOURCES += $(grid_provider_SOURCES) $(grid_information_SOURCES) $(grid_output_pgf_SOURCES)

if HAVE_DUNE_FEM
	fem_error_SOURCES = fem_error.cc ${GTEST_SRC}
	fem_projection_SOURCES = fem_projection.cc ${GTEST_SRC}
	NORMALTESTS += fem_error fem_projection
endif
SOURCES += $(fem_error_SOURCES) $(fem_projection_SOURCES) 
	
# list of tests to run
TESTS = $(NORMALTESTS) compile_pgf_output.sh

# programs just to build when "make check" is used
check_PROGRAMS = $(NORMALTESTS)

EXTRA_DIST = $(SOURCES)

CLEANFILES = *.gcda *.gcno semantic.cache simplex-testgrid*.dgf.* cube-testgrid*.dgf.* dgfparser.log

include $(top_srcdir)/am/global-rules

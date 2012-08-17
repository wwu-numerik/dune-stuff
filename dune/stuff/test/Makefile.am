SUBDIRS = gtest .

GRIDTYPE=YASPGRID
GRIDDIM=2

# tests where program to build and program to run are equal
NORMALTESTS = common_parameter_validation common_logger common_color \
	common_math common_profiler common_string common_typenames \
	function_expression \
	grid_provider grid_information \
	fem_error fem_projection

# list of tests to run
TESTS = $(NORMALTESTS)

# programs just to build when "make check" is used
check_PROGRAMS = $(NORMALTESTS)


LDFLAGS = -lboost_filesystem -lboost_system -lgtest -Lgtest/lib
CXXFLAGS = $(BOOST_CPPFLAGS) $(DUNE_CPPFLAGS) \
	-DGRIDDIM=$(GRIDDIM) -D$(GRIDTYPE) -Igtest/include

common_parameter_validation_SOURCES = common_parameter_validation.cc
common_logger_SOURCES = common_logger.cc
common_math_SOURCES = common_math.cc
common_profiler_SOURCES = common_profiler.cc
common_string_SOURCES = common_string.cc
common_typenames_SOURCES = common_typenames.cc
common_color_SOURCES = common_color.cc

function_expression_SOURCES = function_expression.cc

grid_provider_SOURCES = grid_provider.cc
grid_information_SOURCES = grid_information.cc
grid_output_pgf_SOURCES = grid_output_pgf.cc

fem_error_SOURCES = fem_error.cc
fem_projection_SOURCES = fem_projection.cc

## distribution tarball
SOURCES = $(common_parameter_validation_SOURCES) $(common_logger_SOURCES)

# gridcheck not used explicitly, we should still ship it :)
EXTRA_DIST = $(SOURCES)

CLEANFILES = *.gcda *.gcno semantic.cache simplex-testgrid*.dgf.* cube-testgrid*.dgf.* dgfparser.log

include $(top_srcdir)/am/global-rules

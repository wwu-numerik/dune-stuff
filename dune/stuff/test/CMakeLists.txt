# This file is part of the dune-stuff project:
#   https://github.com/wwu-numerik/dune-stuff
# Copyright holders: Rene Milk, Felix Schindler
# License: BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)

enable_testing()

DEPENDENCYCHECK( ${stuff} )

set_source_files_properties( ${DUNE_HEADERS} PROPERTIES HEADER_FILE_ONLY 1 )

BEGIN_TESTCASES(dunestuff)
find_program(LATEX_FOUND latex)
if (LATEX_FOUND)
  include(UseLATEX)
  if(dune_grid-FOUND)
    # these inputs can only exist if tests have run that require dune-grid
    set( grid_texfiles pgfoutput_stacked.tex pgfoutput_leaf.tex pgfoutput_refineseries.tex)
    add_custom_command(OUTPUT ${grid_texfiles} COMMAND "./test_grid_output_pgf" DEPENDS test_grid_output_pgf)
  endif()
#
endif()

END_TESTCASES()

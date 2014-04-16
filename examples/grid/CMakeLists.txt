# This file is part of the dune-stuff project:
#   http://users.dune-project.org/projects/dune-stuff
# Copyright holders: Rene Milk, Felix Schindler
# License: BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)

ADD_EXECUTABLE( examples_grid_providers "providers.cc" ${COMMON_HEADER} )
dune_target_link_libraries( examples_grid_providers dunestuff )

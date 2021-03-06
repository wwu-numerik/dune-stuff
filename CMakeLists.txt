# This file is part of the dune-stuff project:
#   https://github.com/wwu-numerik/dune-stuff
# The copyright lies with the authors of this file (see below).
# License: BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)
# Authors:
#   Felix Schindler (2012 - 2015)
#   Rene Milk       (2010 - 2015)
#   Sven Kaulmann   (2013)

project("dune-stuff" CXX)

# local environment
set( ENV{PATH} "${CMAKE_CURRENT_SOURCE_DIR}/../local/bin:$ENV{PATH}" )
set( ENV{LD_LIBRARY_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/../local/lib:$ENV{LD_LIBRARY_PATH}" )
set( ENV{PKG_CONFIG_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/../local/lib/pkgconfig:$ENV{PKG_CONFIG_PATH}" )

# cmake specific
cmake_minimum_required( VERSION 3.1 )

set( CMAKE_COLOR_MAKEFILE ON)

# guess dune-common build dir
if(NOT (dune-common_DIR OR dune-common_ROOT OR
        "${CMAKE_PREFIX_PATH}" MATCHES ".*dune-common.*"))
    string(REPLACE  ${CMAKE_PROJECT_NAME} dune-common dune-common_DIR
    ${PROJECT_BINARY_DIR})
endif()

#find dune-common and set the module path
find_package(dune-common REQUIRED)
list(APPEND CMAKE_MODULE_PATH ${dune-common_MODULE_PATH}
"${PROJECT_SOURCE_DIR}/cmake/modules")

#include the dune macros
include(DuneMacros)

# start a dune project with information from dune.module
dune_project()

# dune-stuff cmake includes
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake )

#----------------------------------------------------------------------------------------------------
# General Settings
#----------------------------------------------------------------------------------------------------
execute_process(
    COMMAND ${CMAKE_SOURCE_DIR}/getRevision.sh
    OUTPUT_VARIABLE COMMIT
    ERROR_VARIABLE shell_error
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

SET( DS_FLAGS_Release
  "-fPIC"
  CACHE STRING "")
set(DS_FLAGS_RelWithDebInfo
  "-fPIC"
  CACHE STRING "")
SET( DS_FLAGS_Debug
  "-O0" "-g3" "-ggdb" "-Wunused-variable" "-fno-strict-aliasing" "-fPIC" "-Wall" "-Wextra" "-Wparentheses" "-pedantic"  "-Wshadow" "-Wundef" "-Wnon-virtual-dtor" 
  CACHE STRING "")

#aka anything but intel, but cmake has identifier for icc
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang" OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
  list( APPEND DS_FLAGS_Debug
         -Wc++0x-compat -Wredundant-decls -Winline  -ftemplate-backtrace-limit=0
      )
else ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang" OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
  # for us effectively icc
  list( APPEND DS_FLAGS_Debug
        -Wcast-qual -Wformat=2 -Winit-self -Woverloaded-virtual -Wshadow -Wsign-conversion -Wsign-promo -Wundef  -Wno-unused)
endif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang" OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
  list( APPEND DS_FLAGS_Debug
        -Wnon-literal-null-conversion -Wused-but-marked-unused -Wno-tautological-compare -Wfloat-equal -Wdisabled-macro-expansion -Wcovered-switch-default -Wswitch-enum -Wunreachable-code -Wshorten-64-to-32
      )
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
  list( APPEND DS_FLAGS_Debug
        -Wlogical-op  -Og 
      )
endif()

dune_enable_all_packages(INCLUDE_DIRS ${dune-stuff_SOURCE_DIR}/dune
                         COMPILE_DEFINITIONS DNDEBUG DDNDEBUG
                         COMPILE_OPTIONS ${DS_FLAGS_${CMAKE_BUILD_TYPE}}
                         MODULE_LIBRARIES dunestuff gtest_dune_stuff
                         VERBOSE
                         )

include(DuneUtils)
include(GridUtils)

# header
file( GLOB_RECURSE stuff "${CMAKE_CURRENT_SOURCE_DIR}/dune/*.hh" )
set( COMMON_HEADER ${stuff} ${DUNE_HEADERS} )

# add header of dependent modules for header listing
foreach(_mod ${ALL_DEPENDENCIES})
file(GLOB_RECURSE HEADER_LIST "${CMAKE_CURRENT_SOURCE_DIR}/../${_mod}/*.hh")
list(APPEND COMMON_HEADER ${HEADER_LIST})
endforeach(_mod DEPENDENCIES)
set_source_files_properties(${COMMON_HEADER} PROPERTIES HEADER_FILE_ONLY 1)

#disable most warnings from dependent modules
foreach(_mod ${ALL_DEPENDENCIES})
    dune_module_to_uppercase(_upper_case "${_mod}")
    if(${_mod}_INCLUDE_DIRS)
        foreach( _idir ${${_mod}_INCLUDE_DIRS} )
            add_definitions("-isystem ${_idir}")
        endforeach( _idir )
    endif(${_mod}_INCLUDE_DIRS)
endforeach(_mod DEPENDENCIES)

add_subdirectory(dune)
add_subdirectory("cmake/modules")
add_subdirectory(doc/doxygen)

finalize_dune_project(GENERATE_CONFIG_H_CMAKE)

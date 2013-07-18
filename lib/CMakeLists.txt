dune_add_library(dunestuff SOURCES  ${CMAKE_SOURCE_DIR}/dune/stuff/common/filesystem.cc 
  ${CMAKE_SOURCE_DIR}/dune/stuff/common/color.cc 
  ${CMAKE_SOURCE_DIR}/dune/stuff/common/logging.cc 
  ${CMAKE_SOURCE_DIR}/dune/stuff/common/logstreams.cc
  ${CMAKE_SOURCE_DIR}/dune/stuff/common/profiler.cc
  ${CMAKE_SOURCE_DIR}/dune/stuff/common/parameter/configcontainer.cc
  ${CMAKE_SOURCE_DIR}/dune/stuff/common/parameter/tree.cc
  ${CMAKE_SOURCE_DIR}/dune/stuff/common/signals.cc
  ${CMAKE_SOURCE_DIR}/dune/stuff/common/math.cc
  ${CMAKE_SOURCE_DIR}/dune/stuff/fem/femeoc.cc
  ${CMAKE_SOURCE_DIR}/dune/stuff/function/expression/mathexpr.cc
  )
target_link_dune_default_libraries(dunestuff)
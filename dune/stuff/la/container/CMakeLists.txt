find_package(PythonLibs)
find_package(PythonInterp)
include(CheckPythonModule)
CheckPythonModule(pybindgen)
CheckPythonModule(pygccxml)

if( PYTHONLIBS_FOUND AND PYTHONINTERP_FOUND AND python_pybindgen_FOUND AND python_pygccxml_FOUND )
    message( STATUS "Python bindings will be created" )

    set(pythonTarget           dunecontainermanager)
    set(pythonWrapperGenerator "${CMAKE_CURRENT_SOURCE_DIR}/binding_generator.py")
    set(pythonWrapperTest      "${CMAKE_CURRENT_SOURCE_DIR}/binding_test.py")
    set(pythonWrapperCpp       "${CMAKE_CURRENT_BINARY_DIR}/dunecontainermanager.cc")
    set_source_files_properties( "${pythonWrapperCpp}" PROPERTIES GENERATED TRUE)

    add_custom_command(
        OUTPUT
            "${pythonWrapperCpp}"
        COMMAND
            "${PYTHON_EXECUTABLE}" "${pythonWrapperGenerator}"
            "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_BINARY_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}" "${DUNE_MODULE_DIRS}"
        DEPENDS
            "${pythonWrapperGenerator}"
        WORKING_DIRECTORY
            "${CMAKE_CURRENT_BINARY_DIR}"
        COMMENT
            "  ${pythonTarget}: Generating bindings" VERBATIM
    )
    add_custom_target( "test-${pythonTarget}"
        COMMAND
            "${PYTHON_EXECUTABLE}" "${pythonWrapperTest}"
        DEPENDS
            ${pythonTarget}
        WORKING_DIRECTORY
            "${CMAKE_CURRENT_BINARY_DIR}"
    )

    add_library( dunestufflacontainermanager STATIC "manager.cc" )
    include_directories( ${PYTHON_INCLUDE_DIRS} "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_SOURCE_DIR}")
    add_library( ${pythonTarget} MODULE "${pythonWrapperCpp}" )
    set_target_properties( ${pythonTarget} PROPERTIES PREFIX "" )
    target_link_libraries( ${pythonTarget} dunestufflacontainermanager ${COMMON_LIBS} )

else  (PYTHONLIBS_FOUND AND PYTHONINTERP_FOUND AND python_pybindgen_FOUND AND python_pygccxml_FOUND)
    message(STATUS "Python bindings will NOT be created")
    if( NOT PYTHONLIBS_FOUND )
        message( STATUS "\t because python libs were not found" )
    endif( NOT PYTHONLIBS_FOUND )
    if( NOT PYTHONINTERP_FOUND )
        message( STATUS "\t because python interpreter was not found" )
    endif( NOT PYTHONINTERP_FOUND )
    if( NOT python_pybindgen_FOUND )
        message( STATUS "\t because pybindgen was not found" )
    endif( NOT python_pybindgen_FOUND )
    if( NOT python_pygccxml_FOUND )
        message( STATUS "\t because pygccxml was not found" )
    endif( NOT python_pygccxml_FOUND )
endif( PYTHONLIBS_FOUND AND PYTHONINTERP_FOUND AND python_pybindgen_FOUND AND python_pygccxml_FOUND )

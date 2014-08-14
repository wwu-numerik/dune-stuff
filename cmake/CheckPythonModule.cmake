# This file is part of the dune-stuff project:
#   https://github.com/wwu-numerik/dune-stuff
# Copyright holders: Rene Milk, Felix Schindler
# License: BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)

# check whether given python module exists

macro(CheckPythonModule module)
	execute_process(
		COMMAND
			${PYTHON_EXECUTABLE} "${PROJECT_SOURCE_DIR}/../dune-stuff/cmake/CheckPythonModule.py" "${module}"
		WORKING_DIRECTORY
			${CMAKE_CURRENT_SOURCE_DIR}
		RESULT_VARIABLE
			python_${module}_RESULT_VAR
		)
	IF( python_${module}_RESULT_VAR EQUAL 0 )
		set( python_${module}_FOUND 1 )
	ENDIF( python_${module}_RESULT_VAR EQUAL 0 )
endmacro(CheckPythonModule)

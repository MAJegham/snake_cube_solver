
cmake_minimum_required (VERSION 3.14.0)

set (LIB_SRCS
	${CMAKE_CURRENT_SOURCE_DIR}/exe/sudoku.cc)

add_executable(snake_cube ${LIB_SRCS})
set_target_properties(snake_cube PROPERTIES
                    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
                    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
                    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
target_link_libraries(snake_cube ortools::ortools)
# target_include_directories(sudoku PUBLIC ${ORTOOLS_INCLUDE_DIRS})

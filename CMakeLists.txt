cmake_minimum_required(VERSION 3.14.0)
project(snake_cube_solver VERSION 1.0)

#pathTo or-tools/build/dependencies/install
# list(APPEND CMAKE_PREFIX_PATH ${ORTOOLS_DEPENDENCIES_PATH})
LIST (APPEND CMAKE_MODULE_PATH 0 ${PROJECT_SOURCE_DIR}/cmake)

find_package (ortools REQUIRED)
add_definitions (${ORTOOLS_DEFINITIONS})

# find_package(ZLIB REQUIRED)
# set(GFLAGS_USE_TARGET_NAMESPACE TRUE) 
# find_package(gflags REQUIRED CONFIG) 

# find_package(ortools CONFIG REQUIRED)

add_executable(snake_cube sudoku.cc)
target_link_libraries(snake_cube ortools::ortools)
# target_include_directories(sudoku PUBLIC ${ORTOOLS_INCLUDE_DIRS})

message("done")

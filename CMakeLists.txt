cmake_minimum_required(VERSION 3.14.0)
project(snake_cube_solver 
            VERSION 1.0
            LANGUAGES CXX)

#pathTo or-tools/build/dependencies/install
# list(APPEND CMAKE_PREFIX_PATH ${ORTOOLS_DEPENDENCIES_PATH})
LIST (APPEND CMAKE_MODULE_PATH 0 ${PROJECT_SOURCE_DIR}/cmake)

find_package (ortools REQUIRED)
add_definitions (${ORTOOLS_DEFINITIONS})

add_subdirectory( src )
cmake_minimum_required(VERSION 3.0.2)
project(myproj VERSION 1.0)

#pathTo or-tools/build/dependencies/install
list(APPEND CMAKE_PREFIX_PATH ${ORTOOLS_DEPENDENCIES_PATH})

find_package(ZLIB REQUIRED)
set(GFLAGS_USE_TARGET_NAMESPACE TRUE) 
find_package(gflags REQUIRED CONFIG) 

find_package(ortools CONFIG REQUIRED)

add_executable(sudoku sudoku.cc)
target_link_libraries(sudoku ortools::ortools)
target_include_directories(sudoku PRIVATE ${ORTOOLS_DEPENDENCIES_PATH})

message("done")

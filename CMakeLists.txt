cmake_minimum_required(VERSION 3.18)

project(PSMC LANGUAGES C CXX)

set(CMAKE_STATIC_LIBRARY_PREFIX "") 
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

add_subdirectory(libs)
add_subdirectory(single)

find_package(OpenMP)
if(OpenMP_FOUND)
    add_subdirectory(openmp)
endif()

find_package(MPI)
if(MPI_FOUND)
    add_subdirectory(mpi)
endif()

find_package(CUDAToolkit)
if(CUDAToolkit_FOUND)
    add_subdirectory(cuda)
endif()
cmake_minimum_required(VERSION 3.18)

project(cuda LANGUAGES CXX CUDA)

set(CMAKE_CUDA_FLAGS "${CMAKE_CUDA_FLAGS} --compiler-options=\"-O3 -Wall -Wextra -Wno-implicit-fallthrough -Werror\"")

include_directories(include ../libs/include)

set_source_files_properties(../libs/src/des.c PROPERTIES LANGUAGE CUDA)
add_library(des-cuda ../libs/src/des.c)

add_library(descrack-cuda-messages src/descrack_cuda_messages.cpp)

add_executable(descrack-cuda src/descrack_cuda.cu)
target_link_libraries(descrack-cuda descrack-cuda-messages des-cuda input-descrack CUDA::cudart)

set_target_properties(descrack-cuda des-cuda PROPERTIES CUDA_SEPARABLE_COMPILATION ON)
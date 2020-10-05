cmake_minimum_required(VERSION 3.18)

project(mpi LANGUAGES C)

add_compile_options(-O3 -Wall -Wextra -Wpedantic -Werror)

include_directories(../libs/include)

add_executable(descrack-mpi src/descrack_mpi.c)
target_link_libraries(descrack-mpi helper-descrack input-descrack MPI::MPI_C)
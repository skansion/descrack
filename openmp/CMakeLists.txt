cmake_minimum_required(VERSION 3.18)

project(openmp LANGUAGES C)

add_compile_options(-O3 -Wall -Wextra -Wpedantic -Werror)

include_directories(../libs/include)

add_executable(descrack-openmp src/descrack_openmp.c)
target_link_libraries(descrack-openmp helper-descrack input-descrack OpenMP::OpenMP_C)
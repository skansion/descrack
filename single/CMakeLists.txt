cmake_minimum_required(VERSION 3.18)

project(single LANGUAGES C)

add_compile_options(-O3 -Wall -Wextra -Wpedantic -Werror)

include_directories(../libs/include)

add_executable(descrack src/descrack.c)
target_link_libraries(descrack helper-descrack input-descrack)

add_executable(desencrypt src/desencrypt.c)
target_link_libraries(desencrypt des input-desencrypt)
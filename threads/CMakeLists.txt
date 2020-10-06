cmake_minimum_required(VERSION 3.18)

project(threads LANGUAGES C)

add_compile_options(-O3 -Wall -Wextra -Wpedantic -Werror)

include_directories(../libs/include)

add_executable(descrack-threads src/descrack_threads.c)
target_link_libraries(descrack-threads helper-descrack input-descrack Threads::Threads)
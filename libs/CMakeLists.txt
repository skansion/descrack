cmake_minimum_required(VERSION 3.18)

project(libs LANGUAGES C)

include_directories(include)

add_compile_options(-O3 -Wall -Wextra -Wpedantic -Wno-implicit-fallthrough -Werror)

add_library(des src/des.c)

add_library(helper-descrack src/helper_descrack.c)
target_link_libraries(helper-descrack des)

add_library(input-common src/input_common.c)

add_library(input-messages src/input_messages.c)

add_library(input-descrack src/input_descrack.c)
target_link_libraries(input-descrack input-common input-messages)

add_library(input-desencrypt src/input_desencrypt.c)
target_link_libraries(input-desencrypt input-common input-messages)
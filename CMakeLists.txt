cmake_minimum_required(VERSION 3.16)
project(oop_labs LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_C_STANDARD 11)

add_compile_options(-Wall -Wextra -Wpedantic -g)
#add_link_options(-fsanitize=address -fno-omit-frame-pointer)

enable_testing()

add_subdirectory(lab2)


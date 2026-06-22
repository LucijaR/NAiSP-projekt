cmake_minimum_required(VERSION 3.15)
project(LinearFramework C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

include_directories(include)

file(GLOB_RECURSE FRAMEWORK_SOURCES "src/*.c")

add_library(linear_framework ${FRAMEWORK_SOURCES})

add_executable(test_program tests/main_test.c)
target_link_libraries(test_program linear_framework)

add_executable(int_test_program tests/int_test.c)
target_link_libraries(int_test_program linear_framework)

add_executable(string_test_program tests/string_test.c)
target_link_libraries(string_test_program linear_framework)

add_executable(custom_type_test_program tests/custom_type_test.c)
target_link_libraries(custom_type_test_program linear_framework)

enable_testing()
add_test(NAME int_test COMMAND int_test_program)
add_test(NAME string_test COMMAND string_test_program)
add_test(NAME custom_type_test COMMAND custom_type_test_program)
add_executable(demo_program main.c)
target_link_libraries(demo_program PRIVATE linear_framework)
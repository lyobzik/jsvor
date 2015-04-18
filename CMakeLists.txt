cmake_minimum_required(VERSION 2.8)

set(PROJECT jsvor_root)

project(${PROJECT})

add_subdirectory(lib)
add_subdirectory(tests_common)
add_subdirectory(tests)
add_subdirectory(perftests)
add_subdirectory(jsvor_validator)

cmake_minimum_required(VERSION 3.16)
project(yadro-test-work)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(tape_sorter
    main.cpp
    src/file_tape.cpp
    src/tape_algo.cpp
    src/config_parser.cpp
)
if (CMAKE_BUILD_TYPE MATCHES "DEBUG")
    target_compile_options(tape_sorter PRIVATE -Wall -Wextra -Werror)
endif()

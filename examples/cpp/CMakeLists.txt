cmake_minimum_required(VERSION 3.10)
project(OSQP_Example LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 11)
file(GLOB OSQP_SOURCES "../../osqp/src/*.c")

add_executable(OSQP_Example
    main.cpp
    ${OSQP_SOURCES}
)

target_include_directories(OSQP_Example PRIVATE "../../osqp/include")
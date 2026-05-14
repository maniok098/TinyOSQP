cmake_minimum_required(VERSION 3.10)
project(OSQP_StaticLib LANGUAGES C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(OSQP_ROOT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../osqp")

file(GLOB OSQP_SOURCES "${OSQP_ROOT_DIR}/src/*.c")
add_library(osqp STATIC ${OSQP_SOURCES})

target_include_directories(osqp PUBLIC "${OSQP_ROOT_DIR}/include")

set_target_properties(osqp PROPERTIES OUTPUT_NAME osqp)

install(TARGETS osqp ARCHIVE DESTINATION lib)
install(DIRECTORY ${OSQP_ROOT_DIR}/include/
        DESTINATION include
        FILES_MATCHING PATTERN "*.h"
)

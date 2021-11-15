cmake_minimum_required(VERSION 3.20)
project(IDEF0_SVG_Batch C)

set(CMAKE_C_STANDARD 99)

file(GLOB_RECURSE SOURCES ${PROJECT_SOURCE_DIR}/src/*.*)

add_executable(IDEF0_SVG_Batch ${SOURCES})
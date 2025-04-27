cmake_minimum_required(VERSION 3.12)
project(webserver C)

set(CMAKE_C_STANDARD 23)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

# Recursively find all .c files in src/
file(GLOB_RECURSE SOURCES "src/*.c")

# Recursively find all directories for includes
file(GLOB_RECURSE HEADERS "src/*.h")
foreach(header ${HEADERS})
    get_filename_component(DIR ${header} DIRECTORY)
    list(APPEND INCLUDE_DIRS ${DIR})
endforeach()

list(REMOVE_DUPLICATES INCLUDE_DIRS)
include_directories(${INCLUDE_DIRS})

add_executable(webserver ${SOURCES})

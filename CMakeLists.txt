cmake_minimum_required(VERSION 3.12)

set(PICO_BOARD pico2)

include($ENV{PICO_SDK_PATH}/pico_sdk_init.cmake)

project(picoflight C CXX ASM)

pico_sdk_init()

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/test)

cmake_minimum_required(VERSION 3.21)
set(CMAKE_C_STANDARD 23)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
idf_build_set_property(MINIMAL_BUILD ON)

project(iris_firmware)

cmake_minimum_required(VERSION 3.9)
project(gopro_imu_extract)

set(CMAKE_CXX_STANDARD 14)

# GoPro GPMF parser sources
set(GPMF_SOURCES deps/gpmf-parser/GPMF_parser.c deps/gpmf-parser/demo/GPMF_mp4reader.c)
include_directories(deps/gpmf-parser/ deps/gpmf-parser/demo)

# Build python module
add_subdirectory(deps/pybind11)
pybind11_add_module(gpmfstream src/py_gpstream.cc src/extractor.cc ${GPMF_SOURCES})

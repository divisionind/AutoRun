cmake_minimum_required(VERSION 3.15)
project(AutoRun2)

set(CMAKE_CXX_STANDARD 14)

# allows us to use direct paths in includes (e.g. "systemtray.h" instead of "../include/systemtray.h")
include_directories(src include)

set(SRC_LIST src/main.cpp include/vkarray.h src/vkarray.cpp include/resource.h AutoRun.rc src/systemtray.c include/systemtray.h include/autorun_error.h)

#if cmake is run with the -DRELEASE_BUILD=1 flag, this will evaluate true
if(RELEASE_BUILD)
    add_compile_options("-MT")
    add_executable(AutoRun2 WIN32 ${SRC_LIST})
else()
    add_definitions(-DAUTORUN_DEBUG)
    add_executable(AutoRun2 ${SRC_LIST})
endif()
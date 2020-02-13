cmake_minimum_required(VERSION 3.1)
project(AutoRun)

# version info for building
set(V1 2020)
set(V2 1)
set(V3 3)

set(CMAKE_CXX_STANDARD 14)

# allows us to use direct paths in includes (e.g. "systemtray.h" instead of "../include/systemtray.h")
include_directories(src include)

set(SRC_LIST src/main.cpp include/vkarray.h src/vkarray.cpp include/resource.h AutoRun.rc src/systemtray.c include/systemtray.h include/autorun_error.h)
add_definitions(-DAUTORUN_V1=${V1})
add_definitions(-DAUTORUN_V2=${V2})
add_definitions(-DAUTORUN_V3=${V3})
add_definitions(-DAUTORUN_VERSION_ST="${V1}.${V2}.${V3}")

#if cmake is run with the -DRELEASE_BUILD=1 flag, this will evaluate true
if(RELEASE_BUILD)
    add_compile_options("-MT")
    add_executable(AutoRun WIN32 ${SRC_LIST})
else()
    add_definitions(-DAUTORUN_DEBUG)
    add_executable(AutoRun ${SRC_LIST})
endif()
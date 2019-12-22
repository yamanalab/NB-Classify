cmake_minimum_required(VERSION 2.8)

set(project_name nb-classify)
project(${project_name})

find_package(Threads REQUIRED)
set(CMAKE_CXX_FLAGS "-O3 -std=c++11 -pthread -Wall -DNDEBUG")
set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g -std=c++11 -pthread -Wall")

set(HELIB_DIR /usr/local/helib_pack)
include_directories(${PROJECT_SOURCE_DIR}/nbc ${PROJECT_SOURCE_DIR}/stdsc ${HELIB_DIR}/include)
link_directories(${HELIB_DIR}/lib)

option(TEST_MODE "run on test mode" OFF)
if(TEST_MODE)
  message(STATUS "Enabled test mode.")
  add_definitions(-DENABLE_TEST_MODE)
endif()

if(NOT DEFINED ALGO)
  set(ALGO Single)
endif()

if(ALGO STREQUAL SingleOpt)
  add_definitions(-DUSE_SINGLE_OPT)
  message(STATUS "The classification algorism was set to SingleOpt.")
elseif(ALGO STREQUAL Multi)
  add_definitions(-DUSE_MULTI)
  message(STATUS "The classification algorism was set to Multi.")
else()
  message(STATUS "The classification algorism was set to Single.")
endif()

set(COMMON_LIBS stdsc nbc_share helib ntl gmp)

add_subdirectory(stdsc)
add_subdirectory(nbc)
add_subdirectory(demo)

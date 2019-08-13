cmake_minimum_required(VERSION 3.5 FATAL_ERROR)

## Use -std=c++11 as default.
set(CMAKE_CXX_STANDARD 11)
## Disable C++ extensions
set(CMAKE_CXX_EXTENSIONS OFF)
## Require full C++ standard
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS_RELEASE "-O2 -DNDEBUG -pthread -DFHE_THREADS -DFHE_BOOT_THREADS -fmax-errors=2")

project(helib_example
       	LANGUAGES CXX)

find_package(helib 1.0.0 EXACT REQUIRED)
find_package(Threads REQUIRED)

add_executable(setup setup.cpp)
target_link_libraries(setup helib)
target_link_libraries(setup Threads::Threads)

add_executable(encrypt_model encrypt_model.cpp tools.cpp)
target_link_libraries(encrypt_model helib)
target_link_libraries(encrypt_model Threads::Threads)

add_executable(classifySingle classifySingle.cpp tools.cpp)
target_link_libraries(classifySingle helib)
target_link_libraries(classifySingle Threads::Threads)

add_executable(classifySingleOpt classifySingleOpt.cpp tools.cpp)
target_link_libraries(classifySingleOpt helib)
target_link_libraries(classifySingleOpt Threads::Threads)

add_executable(classifyMulti classifyMulti.cpp tools.cpp)
target_link_libraries(classifyMulti helib)
target_link_libraries(classifyMulti Threads::Threads)

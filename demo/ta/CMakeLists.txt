file(GLOB sources *.cpp)

set(name ta)
add_executable(${name} ${sources})

target_link_libraries(${name} nbc_ta ${COMMON_LIBS})

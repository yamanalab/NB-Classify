file(GLOB sources *.cpp)

set(name mp)
add_executable(${name} ${sources})

target_link_libraries(${name} nbc_mp ${COMMON_LIBS})

file(GLOB sources *.cpp)

set(name cs)
add_executable(${name} ${sources})

target_link_libraries(${name} nbc_cs ${COMMON_LIBS})

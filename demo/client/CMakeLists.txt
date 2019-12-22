file(GLOB sources *.cpp)

set(name client)
add_executable(${name} ${sources})

target_link_libraries(${name} nbc_client ${COMMON_LIBS})

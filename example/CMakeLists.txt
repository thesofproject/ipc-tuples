add_subdirectory(fw)
add_subdirectory(sw)

add_executable(ipct-streamtest test.c)
target_compile_options(ipct-streamtest PUBLIC -g -Wall -Werror)

target_include_directories(ipct-streamtest PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(ipct-streamtest PUBLIC ipct fw-stream sw-stream ipct)
add_library(ipct STATIC pack.c unpack.c client.c)

target_include_directories(ipct PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_compile_options(ipct PUBLIC -g -Wall -Werror)

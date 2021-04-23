add_library(sw-stream STATIC stream.c stream-generated-ipc.c)

target_include_directories(sw-stream  PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_include_directories(sw-stream  PUBLIC ${PROJECT_SOURCE_DIR}/example)
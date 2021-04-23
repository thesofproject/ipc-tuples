add_library(fw-stream STATIC stream.c stream-generated-ipc.c)

target_include_directories(fw-stream  PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_include_directories(fw-stream  PUBLIC ${PROJECT_SOURCE_DIR}/example)
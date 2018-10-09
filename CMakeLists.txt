cmake_minimum_required(VERSION 3.10)
project(tls_perf)

set(CMAKE_C_STANDARD 11)

set(LIB_DIR  /home/rohit/Quic/2018-sergey-masters-code-master/build)

include_directories(${LIB_DIR}/boringssl/include src)
include_directories(${LIB_DIR}/curl-7.58.0/build/include src)
include_directories(/usr/local/include/nghttp2 src)

find_library(SSL ssl HINTS ${LIB_DIR}/boringssl/lib REQUIRED)
find_library(CRYPTO crypto HINTS ${LIB_DIR}/boringssl/lib REQUIRED)
find_library(CURL curl HINTS ${LIB_DIR}/curl-7.58.0/build/lib REQUIRED)
find_library(NGHTTP2 nghttp2 HINTS /usr/local/lib REQUIRED)

add_executable(tls_perf
        src/tls_perf.c
)
target_link_libraries(tls_perf ${CURL} ${SSL} ${CRYPTO} ${NGHTTP2} z m pthread dl)

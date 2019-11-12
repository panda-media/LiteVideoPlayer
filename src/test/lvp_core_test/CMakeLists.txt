cmake_minimum_required(VERSION 3.0)

project(test)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}  -g -Werror   -Wall ")

include_directories("../../../deps/ffmpeg/linux/include/")
link_directories("../../../deps/ffmpeg/linux/lib/")

add_executable(
    test
    ../../core/lvp_core.c
    ../../core/lvp_log.c
    ../../core/lvp_map.c
    ../../core/lvp_event.c
    ../../core/lvp_list.c
    ../../core/lvp_module.c
    ../../core/lvp_thread.c
    ../../core/lvp_mutex.c
    ../../core/lvp_str.c
    ../../core/lvp_queue.c

    ../../reader/lvp_reader_module.c
    ../../filter/lvp_pkt_filter.c
    ../../cache/lvp_cache_module.c
    ../../decoder/lvp_decoder.c
    core_test.c
)

target_link_libraries(
    test
    avformat
    avcodec
    swresample
    swscale
    avutil
    pthread
    m
    z
)
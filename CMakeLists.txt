cmake_minimum_required(VERSION 3.0)

#release
#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}  -Werror -Wunused-parameter -D LVP_WIN32")
#debug
#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}  -g -Wall  -Werror")

project(lvp)


include_directories("./deps/ffmpeg/linux/include/")
link_directories("./deps/ffmpeg/linux/lib/")

include_directories("./deps/ffmpeg/windows/include/")
link_directories("./deps/ffmpeg/windows/lib/")
include_directories("./deps/SDL2/windows/")
link_directories("./deps/SDL2/windows/lib/x64/")

include_directories("./deps/soundtouch/")
link_directories("./deps/soundtouch/")

add_library(lvp 
            ./src/core/lvp_array.c
            ./src/core/lvp_avsync.c
            ./src/core/lvp_condition.c
            ./src/core/lvp_core.c
            ./src/core/lvp_event.c
            ./src/core/lvp_list.c
            ./src/core/lvp_log.c
            ./src/core/lvp_map.c
            ./src/core/lvp_module.c
            ./src/core/lvp_mutex.c
            ./src/core/lvp_nqueue.c
            ./src/core/lvp_queue.c
            ./src/core/lvp_stack.c
            ./src/core/lvp_str.c
            ./src/core/lvp_time.c
            ./src/core/lvp_thread.c
            ./src/core/lvp_cpp.cc

            ./src/decoder/lvp_decoder.c
            ./src/decoder/lvp_hwaccel_decoder.c

            ./src/filter/lvp_audio_resample.c
            ./src/filter/lvp_frame_filter.c
            ./src/filter/lvp_pkt_filter.c
            ./src/filter/lvp_audio_tempo_filter.c

            ./src/reader/lvp_reader_module.c

            ./src/render/lvp_audio_render.c
            ./src/render/lvp_video_render.c

            ./src/cache/lvp_cache_module.c
)

add_executable( LiteVideoPlayer
        ./src/core/lvp.c
)

target_link_libraries(
	lvp

	avformat
    avcodec
    swresample
    swscale
    avutil
    SDL2
    SDL2main
    SoundTouchD_x64
)

target_link_libraries(
    LiteVideoPlayer
    lvp

    avformat
    avcodec
    swresample
    swscale
    avutil
    SDL2
    SDL2main
    SoundTouchD_x64
   # z
   # m
   # pthread
)
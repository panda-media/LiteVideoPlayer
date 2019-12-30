cmake_minimum_required(VERSION 3.0)

#release
#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}  -Werror -Wunused-parameter -D LVP_WIN32")
#debug
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}  -g -Wall  -Werror")

project(lvp)


include_directories("./deps/ffmpeg/linux/include/")
link_directories("./deps/ffmpeg/linux/lib/")

include_directories("./deps/ffmpeg/windows/include/")
link_directories("./deps/ffmpeg/windows/lib/")

include_directories("./deps/ffmpeg/macos/include/")
link_directories("./deps/ffmpeg/macos/lib/")

include_directories("./deps/SDL2/windows/")
link_directories("./deps/SDL2/windows/lib/x64/")

include_directories("./deps/soundtouch/")
link_directories("./deps/soundtouch/")

include_directories("./deps/SDL2/macos/SDL2.framework")
link_directories("./deps/SDL2/macos/SDL2.framework")

include_directories("/usr/local/opt/openssl/include")
link_directories("/usr/local/opt/openssl/lib")

find_library(SDL2 SDL2)
find_library(AVFoundation AVFoundation)
find_library(VideoToolbox VideoToolbox)
find_library(AudioToolbox AudioToolbox)
find_library(CoreFoundation CoreFoundation)
find_library(CoreMedia CoreMedia)
find_library(CoreVideo CoreVideo)
find_library(Security Security)

add_library(lvp SHARED
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
           # ./src/core/lvp_cpp.cc

            ./src/decoder/lvp_decoder.c
            ./src/decoder/lvp_hwaccel_decoder.c

            ./src/filter/lvp_audio_resample.c
            ./src/filter/lvp_frame_filter.c
            ./src/filter/lvp_pkt_filter.c
#            ./src/filter/lvp_audio_tempo_filter.c

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
    ${SDL2}
    ${AVFoundation}
    ${VideoToolbox}
    ${AudioToolbox}
    ${CoreFoundation}
    ${CoreMedia}
    ${CoreVideo}
    ${Security}
    bz2
    iconv
    crypto
    ssl

    z
    m
    pthread
#    SDL2main
#    SoundTouchD_x64
)

target_link_libraries(
    LiteVideoPlayer
    lvp

    avformat
    avcodec
    swresample
    swscale
    avutil
    ${SDL2}
    ${AVFoundation}
    ${VideoToolbox}
    ${AudioToolbox}
    ${CoreFoundation}
    ${CoreMedia}
    ${CoreVideo}
    ${Security}
#    SDL2main
#    SoundTouchD_x64
    bz2
    iconv
    crypto
    ssl

    z
    m
    pthread
)
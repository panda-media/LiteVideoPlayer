cmake_minimum_required(VERSION 3.0)

#release
#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}  -Werror -Wunused-parameter -D LVP_WIN32")
#debug
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}  -g -Wall  -Werror")

project(lvp)

include_directories("./deps/ffmpeg/linux/include")
link_directories("./deps/ffmpeg/linux/lib")

add_executable( lvp
                ./src/core/lvp.c
                ./src/core/lvp_core.c
)
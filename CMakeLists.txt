cmake_minimum_required(VERSION 3.0)

project(lvp)

include_directories("./deps/ffmpeg/linux/include")
link_directories("./deps/ffmpeg/linux/lib")

add_executable( lvp
                ./src/core/lvp.c
                ./src/core/lvp_core.c
)
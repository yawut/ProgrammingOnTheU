cmake_minimum_required(VERSION 3.2)
set(CMAKE_TOOLCHAIN_FILE $ENV{DEVKITPRO}/wut/share/wut.toolchain.cmake)

project(helloworld C)
include("${DEVKITPRO}/wut/share/wut.cmake" REQUIRED)

add_executable(helloworld
    main.c)

target_compile_options(helloworld PRIVATE -Wall)

wut_create_rpx(helloworld)

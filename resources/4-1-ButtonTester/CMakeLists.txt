cmake_minimum_required(VERSION 3.2)
set(CMAKE_TOOLCHAIN_FILE $ENV{DEVKITPRO}/wut/share/wut.toolchain.cmake)

project(buttontest C)
include("${DEVKITPRO}/wut/share/wut.cmake" REQUIRED)

add_executable(buttontest
    main.c)

target_compile_options(buttontest PRIVATE -Wall)

wut_create_rpx(buttontest)

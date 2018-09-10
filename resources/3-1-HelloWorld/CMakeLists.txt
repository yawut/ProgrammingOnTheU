cmake_minimum_required(VERSION 3.2)
set(CMAKE_TOOLCHAIN_FILE $ENV{WUT_ROOT}/share/wut.toolchain.cmake)
include("${WUT_ROOT}/share/wut.cmake" REQUIRED)

project(helloworld C)

add_executable(helloworld
    main.c)

target_link_libraries(helloworld
    coreinit
    whb
    proc_ui
    sysapp)

wut_create_rpx(helloworld.rpx helloworld)

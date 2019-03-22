cmake_minimum_required(VERSION 3.2)
set(CMAKE_TOOLCHAIN_FILE $ENV{WUT_ROOT}/share/wut.toolchain.cmake)

project(buttontest C)
include("${WUT_ROOT}/share/wut.cmake" REQUIRED)

add_executable(buttontest
    main.c)

target_compile_options(buttontest PRIVATE -Wall)

target_link_libraries(buttontest
    coreinit
    whb
    proc_ui
    sysapp
    nsysnet
    vpad)

wut_enable_newlib(buttontest)
wut_create_rpx(buttontest.rpx buttontest)

cmake_minimum_required(VERSION 3.2)
set(CMAKE_TOOLCHAIN_FILE $ENV{WUT_ROOT}/share/wut.toolchain.cmake)

project(helloworld C)
include("${WUT_ROOT}/share/wut.cmake" REQUIRED)

add_executable(helloworld
    main.c)

target_compile_options(helloworld PRIVATE -Wall)

target_link_libraries(helloworld
    coreinit
    whb
    proc_ui
    sysapp
    nsysnet)

wut_enable_newlib(helloworld)
wut_create_rpx(helloworld.rpx helloworld)

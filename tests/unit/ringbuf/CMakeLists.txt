cmake_minimum_required(VERSION 3.20.0)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})

project(ringbuf_test)

target_sources(app PRIVATE
    src/test_ringbuf.c
    ../../../app/src/ringbuf.c
)

target_include_directories(app PRIVATE
    ../../../app/src
)

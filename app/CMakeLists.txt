set(ZEPHYR_USE_RELATIVE_PATHS OFF)
add_compile_options(-fmacro-prefix-map=/=/)
cmake_minimum_required(VERSION 3.20.0)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})

project(sensor_app)

target_sources(app PRIVATE
    src/main.c
    src/ringbuf.c
)

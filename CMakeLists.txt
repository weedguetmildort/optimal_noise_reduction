cmake_minimum_required(VERSION 3.23)
project(optimal_noise_reduction C)

set(CMAKE_C_STANDARD 11)

include_directories(src)

add_executable(optimal_noise_reduction
        src/main.c
        src/main.h)

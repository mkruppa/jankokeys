cmake_minimum_required(VERSION 3.10)
project(jankokeys LANGUAGES C)

set(CMAKE_C_FLAGS "-Wall -Wextra -Werror -O2")

find_package(ALSA REQUIRED)

add_executable(${PROJECT_NAME} src/main.c src/alsa_seq.c)

target_link_libraries(${PROJECT_NAME} ALSA::ALSA)
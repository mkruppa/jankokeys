cmake_minimum_required(VERSION 3.10)
project(jankokeys LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Werror")

find_package(ALSA REQUIRED)
find_package(Lua REQUIRED)
find_package(SDL2 REQUIRED)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GLEW REQUIRED IMPORTED_TARGET glew)
pkg_check_modules(SDL2IMAGE REQUIRED SDL2_image)

include_directories(
	${LUA_INCLUDE_DIR}
	${SDL2IMAGE_INCLUDE_DIRS})

add_executable(${PROJECT_NAME}
	src/main.c
	src/alsa_seq.c
	src/config.c
	src/window.c
	src/shader.c
	src/midi_keyboard_janko.c
	src/midi_process_events.c
	src/texture_atlas.c
	src/sdl_clock.c
	src/easing.c
	src/midi_keyboard_key_color.c
	src/piano_roll.c)

target_link_libraries(${PROJECT_NAME}
	ALSA::ALSA
	${LUA_LIBRARIES}
	SDL2::SDL2
	${SDL2IMAGE_LIBRARIES}
	PkgConfig::GLEW)
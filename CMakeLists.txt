cmake_minimum_required(VERSION 3.10)
project(jankokeys LANGUAGES C)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Werror -O2")

find_package(ALSA REQUIRED)
find_package(Lua REQUIRED)
find_package(SDL2 REQUIRED)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GLEW REQUIRED IMPORTED_TARGET glew)

include_directories(${LUA_INCLUDE_DIR})

add_executable(${PROJECT_NAME}
	src/main.c
	src/alsa_seq.c
	src/config.c
	src/window.c
	src/shader.c
	src/midi_keyboard_janko.c)

target_link_libraries(${PROJECT_NAME}
	ALSA::ALSA
	${LUA_LIBRARIES}
	SDL2::SDL2
	PkgConfig::GLEW)
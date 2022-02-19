#pragma once

#include <stdbool.h>
#include <lua.h>
#include <SDL_scancode.h>

#define NUM_MIDI_NOTES 128

typedef struct {
	int width;
	int height;
	SDL_Scancode sustain_key;
	int keybinding[SDL_NUM_SCANCODES];
} config_t;

void luaJK_stack_dump(lua_State *L);

void luaJK_print_error_var_not_found(const char *var);

bool luaJK_get_global_var_int(int *out, lua_State *L, const char *var);

bool bounds_check_keybinding(int scan_code, int midi_note_number);

bool luaJK_get_global_var_table_int_int(int *out, bool (*bounds_check)(int, int), lua_State *L, const char *var);

bool luaJK_dofile(lua_State *L, const char *file_path);

bool config_load(config_t *config);
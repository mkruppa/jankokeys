#pragma once

#include <stdbool.h>
#include <lua.h>

#define NUM_SCANCODES 512
#define NUM_MIDI_NOTES 128

void lua_stack_dump(lua_State *L);

bool config_load(int keybinding[NUM_SCANCODES]);
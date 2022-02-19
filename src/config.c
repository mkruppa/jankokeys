#include "config.h"

#include <lualib.h>
#include <lauxlib.h>

#define CONFIG_FILE_PATH "../script/config.lua"
#define LUA_GLOBAL_VAR_KEYBINDING "keybinding"

void lua_stack_dump(lua_State *L)
{
	int top = lua_gettop(L);

	printf("Lua stack dump (%d elements):\n", top);
	if (top > 0) {
		printf("\t|  ");
	} else {
		printf("\tempty");
	}

	for (int i = 1; i <= top; ++i) {
		int t = lua_type(L, i);
		switch (t) {
		case LUA_TSTRING:
			printf("'%s'", lua_tostring(L, i));
			break;
		case LUA_TBOOLEAN:
			printf(lua_toboolean(L, i) ? "true" : "false");
			break;
		case LUA_TNUMBER:
			printf("%g", lua_tonumber(L, i));
			break;
		default:
			printf("%s", lua_typename(L, t));
			break;
		}
		printf("  |  ");
	}
	printf("\n");
}

bool config_load(int keybinding[NUM_SCANCODES])
{
	bool config_loaded = false;

	lua_State *L = luaL_newstate();
	luaL_openlibs(L);

	if (luaL_dofile(L, CONFIG_FILE_PATH) != LUA_OK) {
		fprintf(stderr, "ERROR: unable to load config file: %s\n", lua_tostring(L, -1));
		goto error;
	}

	lua_getglobal(L, LUA_GLOBAL_VAR_KEYBINDING);
	if (!lua_istable(L, -1)) {
		fprintf(stderr, "ERROR: global variable '%s' not found\n", LUA_GLOBAL_VAR_KEYBINDING);
		goto error;
	}

	/* lua table traversal */
	lua_pushnil(L);
	while (lua_next(L, -2) != 0) {
		if (lua_isinteger(L, -2) && lua_isinteger(L, -1)) {
			int scan_code = (int)lua_tointeger(L, -2);
			int midi_note_number = (int)lua_tointeger(L, -1);

			/* bounds checks */
			if (scan_code < 0 || scan_code >= NUM_SCANCODES) {
				fprintf(stderr, "ERROR: scan code %d out of bounds\n", scan_code);
				goto error;
			}
			if (midi_note_number < 0 || midi_note_number >= NUM_MIDI_NOTES) {
				fprintf(stderr, "ERROR: midi note number %d out of bounds\n", midi_note_number);
				goto error;
			}

			keybinding[scan_code] = midi_note_number; 
		}
		lua_pop(L, 1);
	}

	config_loaded = true;
error:
	lua_close(L);
	return config_loaded;
}
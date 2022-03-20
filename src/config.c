#include "config.h"

#include <lualib.h>
#include <lauxlib.h>

#define CONFIG_FILE_PATH "../script/config.lua"
#define LUA_GLOBAL_VAR_WIDTH "width"
#define LUA_GLOBAL_VAR_HEIGHT "height"
#define LUA_GLOBAL_VAR_JANKO_NUM_ROWS "janko_num_rows"
#define LUA_GLOBAL_VAR_SUSTAIN_KEY "sustain_key"
#define LUA_GLOBAL_VAR_KEYBINDING "keybinding"

void luaJK_stack_dump(lua_State *L)
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

void luaJK_print_error_var_not_found(const char *var)
{
	fprintf(stderr, "ERROR: global variable '%s' not found\n", var);
}

bool luaJK_get_global_var_int(int *out, lua_State *L, const char *var)
{
	lua_getglobal(L, var);
	if (!lua_isinteger(L, -1)) {
		luaJK_print_error_var_not_found(var);
		return false;
	}

	*out = (int)lua_tointeger(L, -1);
	lua_pop(L, 1);
	return true;
}

bool bounds_check_keybinding(int scan_code, int midi_note_number)
{
	if (scan_code < 0 || scan_code >= SDL_NUM_SCANCODES) {
		fprintf(stderr, "ERROR: scan code %d out of bounds\n", scan_code);
		return false;
	}
	if (midi_note_number < 0 || midi_note_number >= NUM_MIDI_NOTES) {
		fprintf(stderr, "ERROR: midi note number %d out of bounds\n", midi_note_number);
		return false;
	}
	return true;
}

bool luaJK_get_global_var_table_int_int(int *out, bounds_check_t bounds_check, lua_State *L, const char *var)
{
	lua_getglobal(L, var);
	if (!lua_istable(L, -1)) {
		luaJK_print_error_var_not_found(var);
		return false;
	}

	/* lua table traversal */
	lua_pushnil(L);
	while (lua_next(L, -2) != 0) {
		if (lua_isinteger(L, -2) && lua_isinteger(L, -1)) {
			int key = (int)lua_tointeger(L, -2);
			int value = (int)lua_tointeger(L, -1);
			if (!bounds_check(key, value)) { return false; }
			out[key] = value; 
		}
		lua_pop(L, 1);
	}
	lua_pop(L, 1);
	return true;
}

bool luaJK_dofile(lua_State *L, const char *file_path)
{
	bool file_executed = luaL_dofile(L, file_path) == LUA_OK;
	if (!file_executed) { fprintf(stderr, "ERROR: unable to load config file: %s\n", lua_tostring(L, -1)); }
	return file_executed;
}

bool config_load(config_t *config)
{
	lua_State *L = luaL_newstate();
	luaL_openlibs(L);

	bool config_loaded = 
		luaJK_dofile(L, CONFIG_FILE_PATH) &&
		luaJK_get_global_var_int(&config->width, L, LUA_GLOBAL_VAR_WIDTH) &&
		luaJK_get_global_var_int(&config->height, L, LUA_GLOBAL_VAR_HEIGHT) &&
		luaJK_get_global_var_int(&config->janko_num_rows, L, LUA_GLOBAL_VAR_JANKO_NUM_ROWS) &&
		luaJK_get_global_var_int((int*)&config->sustain_key, L, LUA_GLOBAL_VAR_SUSTAIN_KEY) &&
		luaJK_get_global_var_table_int_int((int*)&config->keybinding, &bounds_check_keybinding, L, LUA_GLOBAL_VAR_KEYBINDING);

	lua_close(L);
	return config_loaded;
}
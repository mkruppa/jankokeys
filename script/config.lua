require "math"

-- janko keyboard settings
janko_key_height_over_width_ratio = 3 / 2
janko_num_rows = 4

-- window settings
key_width = 32
num_keys = 88
width = math.floor((num_keys + 1) * key_width / 2)
height = math.floor(janko_num_rows * key_width * janko_key_height_over_width_ratio)
-- height = math.floor(width * (9 / 16))

-- keyboard bindings
dofile("../script/scancodes.lua")

sustain_key = SDL_SCANCODE_SPACE

-- maps SDL scancodes to midi note numbers
keybinding = {
	[SDL_SCANCODE_1] = 58,
	[SDL_SCANCODE_2] = 60,
	[SDL_SCANCODE_3] = 62,
	[SDL_SCANCODE_4] = 64,
	[SDL_SCANCODE_5] = 66,
	[SDL_SCANCODE_6] = 68,
	[SDL_SCANCODE_7] = 70,
	[SDL_SCANCODE_8] = 72,
	[SDL_SCANCODE_9] = 74,
	[SDL_SCANCODE_0] = 76,
	[SDL_SCANCODE_MINUS] = 78,
	[SDL_SCANCODE_EQUALS] = 80,

	[SDL_SCANCODE_Q] = 59,
	[SDL_SCANCODE_W] = 61,
	[SDL_SCANCODE_E] = 63,
	[SDL_SCANCODE_R] = 65,
	[SDL_SCANCODE_T] = 67,
	[SDL_SCANCODE_Y] = 69,
	[SDL_SCANCODE_U] = 71,
	[SDL_SCANCODE_I] = 73,
	[SDL_SCANCODE_O] = 75,
	[SDL_SCANCODE_P] = 77,
	[SDL_SCANCODE_LEFTBRACKET] = 79,
	[SDL_SCANCODE_RIGHTBRACKET] = 81,

	[SDL_SCANCODE_A] = 60,
	[SDL_SCANCODE_S] = 62,
	[SDL_SCANCODE_D] = 64,
	[SDL_SCANCODE_F] = 66,
	[SDL_SCANCODE_G] = 68,
	[SDL_SCANCODE_H] = 70,
	[SDL_SCANCODE_J] = 72,
	[SDL_SCANCODE_K] = 74,
	[SDL_SCANCODE_L] = 76,
	[SDL_SCANCODE_SEMICOLON] = 78,
	[SDL_SCANCODE_APOSTROPHE] = 80,
	[SDL_SCANCODE_BACKSLASH] = 82,

	[SDL_SCANCODE_NONUSBACKSLASH] = 59,
	[SDL_SCANCODE_Z] = 61,
	[SDL_SCANCODE_X] = 63,
	[SDL_SCANCODE_C] = 65,
	[SDL_SCANCODE_V] = 67,
	[SDL_SCANCODE_B] = 69,
	[SDL_SCANCODE_N] = 71,
	[SDL_SCANCODE_M] = 73,
	[SDL_SCANCODE_COMMA] = 75,
	[SDL_SCANCODE_PERIOD] = 77,
	[SDL_SCANCODE_SLASH] = 79,
}
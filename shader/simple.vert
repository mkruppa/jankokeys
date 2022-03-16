#version 330 core
layout (location = 0) in vec3 vert_pos;
layout (location = 1) in vec3 vert_col;
layout (location = 2) in vec2 vert_tex;

out vec3 col;
out vec2 tex;

uniform mat4 M = mat4(1.0);

void main()
{
	col = vert_col;
	tex = vert_tex;
	gl_Position = M * vec4(vert_pos, 1.0);
}
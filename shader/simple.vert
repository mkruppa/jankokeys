#version 330 core
layout (location = 0) in vec3 vert_pos;
layout (location = 1) in vec3 vert_col;

out vec3 col;

uniform mat4 M = mat4(1.0);

void main()
{
	col = vert_col;
	gl_Position = M * vec4(vert_pos, 1.0);
}
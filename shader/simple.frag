#version 330 core

in vec3 col;
in vec2 tex;

uniform sampler2D texture0;

void main()
{
	gl_FragColor = vec4(col, 1.0) * texture(texture0, tex);
}
#version 430 core

in vec4 position;

void main()
{
	vec4 flip = vec4(-1, -1, 1, 1);
	gl_Position = position*flip;
}

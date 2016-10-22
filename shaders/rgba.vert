#version 430 core
// in values
in vec3 position;
layout(location = 1) in vec4 rgba_color;
// out values
out vec4 frag_color;


void main()
{
    gl_Position = vec4(position, 1.0f);
    frag_color = rgba_color;
}

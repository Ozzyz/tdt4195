#version 430 core
// periodic value that varies between -0.5 and 0.5
uniform layout(location = 2) mat4x4 vertexMatrix;
// in values
in vec3 position;
layout(location = 1) in vec4 rgba_color;
// out values
out vec4 frag_color;


void main()
{
    gl_Position = vertexMatrix*vec4(position, 1.0f);
    frag_color = rgba_color;
}

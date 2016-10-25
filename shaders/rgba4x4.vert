#version 430 core
// periodic value that varies between -0.5 and 0.5
uniform layout(location = 2) float value;
// in values
in vec3 position;
layout(location = 1) in vec4 rgba_color;
// out values
out vec4 frag_color;


void main()
{

    mat4 vertexMatrix;
    vertexMatrix[0] = vec4(1,0,0,0);
    vertexMatrix[1] = vec4(0,1,0,value);
    vertexMatrix[2] = vec4(0,0,1,0);
    vertexMatrix[3] = vec4(0,0,0,1);
    gl_Position = vertexMatrix*vec4(position, 1.0f);
    frag_color = rgba_color;
}

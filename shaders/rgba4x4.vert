#version 430 core
// in values
in vec3 position;
layout(location = 1) in vec4 rgba_color;
// out values
out vec4 frag_color;
float vertexMatrix[] = {};
vertexMatrix[0] = vec4(1,0,0,0);
vertexMatrix[1] = vec4(0,1,0,0);
vertexMatrix[2] = vec4(0,0,1,0);
vertexMatrix[3] = vec4(0,0,0,1);
void main()
{
    gl_Position = vec4(position, 1.0f)*vertexMatrix;
    frag_color = rgba_color;
}

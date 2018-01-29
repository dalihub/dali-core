#version 430

layout( location = 0 ) in vec4 aPosition;

layout( set = 0, binding = 0, std140 ) uniform world
{
    mat4 modelMat;
    mat4 viewMat;
    mat4 projMat;
    vec4 color;
};

void main()
{
    gl_Position = projMat * viewMat * modelMat * aPosition;
}
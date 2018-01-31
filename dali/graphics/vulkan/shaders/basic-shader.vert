#version 430

layout( location = 0 ) in vec3 aPosition;

layout( set = 0, binding = 0, std140 ) uniform world
{
    mat4 modelMat;
    mat4 viewMat;
    mat4 projMat;
    vec4 color;
};

layout( location = 0 ) out vec4 triColor;

void main()
{
    gl_Position = projMat * viewMat * modelMat * vec4( aPosition, 1.0 );
    triColor = color;
}
#version 430

layout( location = 0 ) in vec4 triColor;
layout( location = 0 ) out vec4 outColor;

void main()
{
    outColor = triColor;
}
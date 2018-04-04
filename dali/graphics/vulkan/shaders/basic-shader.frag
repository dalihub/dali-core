#version 430

layout( location = 0 ) in vec4 triColor;
layout( location = 1 ) in vec2 uvCoords;
layout( location = 0 ) out vec4 outColor;

layout( set = 0, binding = 4 ) uniform sampler2D sTexture;
layout( set = 0, binding = 2 ) uniform sampler2D sTexture2;

void main()
{
    outColor = texture( sTexture, uvCoords) * triColor;
}
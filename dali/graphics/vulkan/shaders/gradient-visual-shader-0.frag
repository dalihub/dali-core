#version 430

layout(location=0) in vec2 vTexCoord;

layout( set = 0, binding = 1, std140 ) uniform FragData
{
    vec4 uColor;
    vec3 mixColor;
};

layout( set = 0, binding = 2 ) uniform sampler2D sTexture; // sampler1D?

layout( location = 0 ) out vec4 fragColor;

void main()
{
    fragColor = texture( sTexture, vec2( vTexCoord.y, 0.5 ) ) * vec4(mixColor, 1.0) * uColor;
}

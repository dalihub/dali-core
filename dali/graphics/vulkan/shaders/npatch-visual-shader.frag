#version 430

layout(location=0) in vec2 vTexCoord;

layout(set=1, binding=0, std140) uniform FragData
{
    vec4 uColor;
    vec3 mixColor;
    float opacity;
    float preMultipliedAlpha;
};

layout(set=1, binding=1) uniform sampler2D sTexture;

layout(location=0) out vec4 fragColor;

vec4 visualMixColor()
{
    return vec4( mixColor * mix( 1.0, opacity, preMultipliedAlpha ), opacity );
}

void main()
{
    fragColor = texture( sTexture, vTexCoord ) * uColor * visualMixColor();
}

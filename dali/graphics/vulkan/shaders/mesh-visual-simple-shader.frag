#version 430

layout(location=0) in vec3 vIllumination;

layout(set=0, binding=1, std140) uniform FragData
{
    vec4 uColor;
    vec3 mixColor;
    float opacity;
    float preMultipliedAlpha;
};

layout(location=0) out vec4 fragColor;

vec4 visualMixColor()
{
    return vec4( mixColor * mix( 1.0, opacity, preMultipliedAlpha ), opacity );
}
void main()
{
    fragColor = vec4( vIllumination.rgb * uColor.rgb, uColor.a ) * visualMixColor();
}

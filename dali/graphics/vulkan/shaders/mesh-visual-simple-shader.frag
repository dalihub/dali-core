#version 430

layout(location=0) in vec3 vIllumination;

layout(set=0, binding=1, std140) uniform FragData
{
    vec4 uColor;
    vec3 mixColor;
};

layout(location=0) out vec4 fragColor;

void main()
{
    fragColor = vec4( vIllumination.rgb * uColor.rgb, uColor.a );// * vec4(mixColor,1.0);
}

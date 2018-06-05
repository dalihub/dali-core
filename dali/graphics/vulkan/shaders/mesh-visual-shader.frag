#version 430

layout(location=0) in vec2 vTexCoord;
layout(location=1) in vec3 vIllumination;
layout(location=2) in float vSpecular;

layout( set = 0, binding = 1, std140 ) uniform FragData
{
    vec4 uColor;
    vec3 mixColor;
};

layout( set = 0, binding = 2 ) uniform sampler2D sDiffuse;

layout( location = 0 ) out vec4 fragColor;

void main()
{
    vec4 tex = texture( sDiffuse, vTexCoord );
    vec4 visualMixColor = vec4(mixColor,1.0);
    fragColor = vec4( vIllumination.rgb * tex.rgb * uColor.rgb * visualMixColor.rgb + vSpecular * 0.3, tex.a * uColor.a * visualMixColor.a );
}

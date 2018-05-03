#version 430

layout(location=0) in vec2 vTexCoord;
layout(location=1) in vec3 vIllumination;
layout(location=2) in float vSpecular;

layout( set = 1, binding = 0, std140 ) uniform FragData
{
    vec4 uColor;
    vec3 mixColor;
    float opacity;
    float preMultipliedAlpha;
};

layout( set = 1, binding = 1 ) uniform sampler2D sDiffuse;

layout( location = 0 ) out vec4 fragColor;

vec4 visualMixColor()
{
    return vec4( mixColor * mix( 1.0, opacity, preMultipliedAlpha ), opacity );
}

void main()
{
    vec4 texture = texture( sDiffuse, vTexCoord );
    vec4 visualMixColor = visualMixColor();
    fragColor = vec4( vIllumination.rgb * texture.rgb * uColor.rgb * visualMixColor.rgb + vSpecular * 0.3, texture.a * uColor.a * visualMixColor.a );
}

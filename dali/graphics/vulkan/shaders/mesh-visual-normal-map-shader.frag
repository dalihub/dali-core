#version 430

layout(location=0) in vec2 vTexCoord;
layout(location=1) in vec3 vLightDirection;
layout(location=2) in vec3 vHalfVector;

layout( set = 0, binding = 1, std140 ) uniform FragData
{
    vec4 uColor;
    vec3 mixColor;
    float opacity;
    float preMultipliedAlpha;
};

layout( set = 0, binding = 2 ) uniform sampler2D sDiffuse;
layout( set = 0, binding = 3 ) uniform sampler2D sNormal;
layout( set = 0, binding = 4 ) uniform sampler2D sGloss;

layout( location = 0 ) out vec4 fragColor;

vec4 visualMixColor()
{
    return vec4( mixColor * mix( 1.0, opacity, preMultipliedAlpha ), opacity );
}
void main()
{
    vec4 diffuse = texture( sDiffuse, vTexCoord );
    vec3 normal = normalize( texture( sNormal, vTexCoord ).xyz * 2.0 - 1.0 );
    vec4 glossMap = texture( sGloss, vTexCoord );
    vec4 visualMixColor = visualMixColor();

    float lightDiffuse = max( 0.0, dot( normal, normalize( vLightDirection ) ) );
    lightDiffuse = lightDiffuse * 0.5 + 0.5;

    float shininess = pow ( max ( dot ( normalize( vHalfVector ), normal ), 0.0 ), 16.0 );

    fragColor = vec4( diffuse.rgb * uColor.rgb * visualMixColor.rgb * lightDiffuse + shininess * glossMap.rgb, diffuse.a * uColor.a * visualMixColor.a );
}

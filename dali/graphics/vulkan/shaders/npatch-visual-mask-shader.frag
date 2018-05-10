#version 430

layout(location=0) in vec2 vTexCoord;
layout(location=1) in vec2 vMaskTexCoord;

layout(set=0, binding=1, std140) uniform FragData
{
    vec4 uColor;
    vec3 mixColor;
    float opacity;
    float preMultipliedAlpha;
    float auxiliaryImageAlpha;
};

layout(set=0, binding=2) uniform sampler2D sTexture;
layout(set=0, binding=3) uniform sampler2D sMask;

layout(location=0) out vec4 fragColor;

void main()
{
  vec4 color = texture( sTexture, vTexCoord );
  vec4 mask  = texture( sMask, vMaskTexCoord );

  vec3 mixedColor = color.rgb * mix( 1.0-mask.a, 1.0, 1.0-auxiliaryImageAlpha) + mask.rgb*mask.a * auxiliaryImageAlpha;

  fragColor = vec4(mixedColor,1.0) * uColor * vec4( mixColor, 1.0 );
}

#version 430

layout( location = 0 ) in vec2 vTexCoord;

// uniforms
layout( set = 0, binding = 1, std140 ) uniform FragData
{
  vec4 uAtlasRect;
  vec4 uColor;
  vec3 mixColor;
  float opacity;
  float preMultipliedAlpha;
};

layout( set = 0, binding = 2 ) uniform sampler2D sTexture;

layout( location = 0 ) out vec4 fragColor;

vec4 visualMixColor()
{
  return vec4( mixColor * mix( 1.0, opacity, preMultipliedAlpha ), opacity );
}

void main()
{
  vec2 texCoord = clamp( mix( uAtlasRect.xy, uAtlasRect.zw, vTexCoord ), uAtlasRect.xy, uAtlasRect.zw );
  vec4 textTexture = texture( sTexture, texCoord );
  textTexture.rgb *= mix( 1.0, textTexture.a, preMultipliedAlpha );

  fragColor = textTexture * uColor * visualMixColor();
}

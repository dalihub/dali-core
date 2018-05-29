#version 430

layout( location = 0 ) in vec2 vTexCoord;

layout( set = 0, binding = 1, std140 ) uniform FragData
{
  vec4 uTextColorAnimatable;
  vec4 uAtlasRect;
  vec4 uColor;
  vec3 mixColor;
  float opacity;
  float preMultipliedAlpha;
};

layout( set = 0, binding = 2) uniform sampler2D sTexture;

layout( location = 0 ) out vec4 fragColor;

vec4 visualMixColor()
{
  return vec4( mixColor * mix( 1.0, opacity, preMultipliedAlpha ), opacity );
}

void main()
{
  vec2 texCoord = clamp( mix( uAtlasRect.xy, uAtlasRect.zw, vTexCoord ), uAtlasRect.xy, uAtlasRect.zw );
  float textTexture = texture( sTexture, texCoord ).r;

  // Set the color of the text to what it is animated to.
  fragColor = uTextColorAnimatable * textTexture * uColor * visualMixColor();
}

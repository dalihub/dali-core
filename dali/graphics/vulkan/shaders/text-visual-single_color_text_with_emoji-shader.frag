#version 430

layout( location = 0 ) in vec2 vTexCoord;

layout( set = 1, binding = 0, std140 ) uniform FragData
{
  vec4 uTextColorAnimatable;
  vec4 uAtlasRect;
  vec4 uColor;
  vec3 mixColor;
  float opacity;
  float preMultipliedAlpha;
};

layout( set = 1, binding = 1 ) uniform sampler2D sTexture;
layout( set = 1, binding = 2 ) uniform sampler2D sMask;

layout( location = 0 ) out vec4 fragColor;

vec4 visualMixColor()
{
  return vec4( mixColor * mix( 1.0, opacity, preMultipliedAlpha ), opacity );
}

void main()
{
  vec2 texCoord = clamp( mix( uAtlasRect.xy, uAtlasRect.zw, vTexCoord ), uAtlasRect.xy, uAtlasRect.zw );
  vec4 textTexture = texture( sTexture, texCoord );
  float maskTexture = texture( sMask, texCoord ).r;

  // Set the color of non-transparent pixel in text to what it is animated to.
  // Markup text with multiple text colors are not animated (but can be supported later on if required).
  // Emoji color are not animated.
  float vstep = step( 0.0001, textTexture.a );
  textTexture.rgb = mix( textTexture.rgb, uTextColorAnimatable.rgb, vstep * maskTexture ) * mix( 1.0, textTexture.a, preMultipliedAlpha );

  // Draw the text as overlay above the style
  fragColor = textTexture * uColor * visualMixColor();
}

#version 430

layout( location = 0 ) in vec2 vTexCoord;

layout( set = 0, binding = 1, std140 ) uniform FragData
{
  vec4 uTextColorAnimatable;
  vec4 uColor;
  vec3 mixColor;
  float preMultipliedAlpha;
};

layout( set = 0, binding = 2 ) uniform sampler2D sTexture;
layout( set = 0, binding = 3 ) uniform sampler2D sMask;

layout( location = 0 ) out vec4 fragColor;

void main()
{
  vec4 theTextTexture = texture( sTexture, vTexCoord );
  float theMaskTexture = texture( sMask, vTexCoord ).r;

  // Set the color of non-transparent pixel in text to what it is animated to.
  // Markup text with multiple text colors are not animated (but can be supported later on if required).
  // Emoji color are not animated.
  float vstep = step( 0.0001, theTextTexture.a );
  theTextTexture.rgb = mix( theTextTexture.rgb, uTextColorAnimatable.rgb, vstep * theMaskTexture ) * mix( 1.0, theTextTexture.a, preMultipliedAlpha );

  // Draw the text as overlay above the style
  fragColor = theTextTexture * uColor * vec4(mixColor,1.0);
}

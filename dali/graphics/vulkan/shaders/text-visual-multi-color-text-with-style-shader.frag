#version 430

layout( location = 0 ) in vec2 vTexCoord;

layout( set = 1, binding = 0, std140 ) uniform FragData
{
  vec4 uColor;
  vec3 mixColor;
  float preMultipliedAlpha;
};

layout( set = 1, binding = 1 ) uniform sampler2D sTexture;
layout( set = 1, binding = 2 ) uniform sampler2D sStyle;

layout( location = 0 ) out vec4 fragColor;

void main()
{
  vec4 textTexture = texture( sTexture, vTexCoord );
  vec4 styleTexture = texture( sStyle, vTexCoord );
  textTexture.rgb *= mix( 1.0, textTexture.a, preMultipliedAlpha );

  // Draw the text as overlay above the style
  fragColor = ( textTexture + styleTexture * ( 1.0 - textTexture.a ) ) * uColor * vec4(mixColor,1.0);
}

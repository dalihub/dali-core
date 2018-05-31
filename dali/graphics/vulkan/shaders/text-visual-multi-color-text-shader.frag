#version 430

layout( location = 0 ) in vec2 vTexCoord;

// uniforms
layout( set = 0, binding = 1, std140 ) uniform FragData
{
  vec4 uColor;
  vec3 mixColor;
  float preMultipliedAlpha;
};

layout( set = 0, binding = 2 ) uniform sampler2D sTexture;

layout( location = 0 ) out vec4 fragColor;

void main()
{
  vec4 textTexture = texture( sTexture, vTexCoord );
  textTexture.rgb *= mix( 1.0, textTexture.a, preMultipliedAlpha );

  fragColor = textTexture * uColor * vec4(mixColor,1.0);
}

#version 430

layout( location=0 ) in vec2 vTexCoord;

layout(set=1, binding=0, std140) uniform FragData
{
  vec4 uAtlasRect;
  vec2 wrapMode;
  vec4 uColor;
  vec3 mixColor;
  float opacity;
  float preMultipliedAlpha;
};

layout(set=1, binding=1) uniform sampler2D sTexture;

layout(location=0) out vec4 fragColor;

float wrapCoordinate( vec2 range, float coordinate, float wrap )
{
  float coord;
  if( wrap > 1.5 ) // REFLECT
    coord = 1.0-abs(fract(coordinate*0.5)*2.0 - 1.0);
  else // warp == 0 or 1
    coord = mix(coordinate, fract( coordinate ), wrap);
  return clamp( mix(range.x, range.y, coord), range.x, range.y );
}

vec4 visualMixColor()
{
  return vec4( mixColor * mix( 1.0, opacity, preMultipliedAlpha ), opacity );
}

void main()
{
    vec2 texCoord = vec2( wrapCoordinate( uAtlasRect.xz, vTexCoord.x, wrapMode.x ),
                                  wrapCoordinate( uAtlasRect.yw, vTexCoord.y, wrapMode.y ) );
    fragColor = texture( sTexture, texCoord ) * uColor * visualMixColor();
}

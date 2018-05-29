#version 430

layout( location = 0 ) in float vAlpha;

layout( set = 1, binding = 0, std140 ) uniform FragData
{
  vec4 uColor;
  vec4 borderColor;
  vec3 mixColor;
  float opacity;
  float borderSize;
};

layout( location = 0 ) out vec4 fragColor;

void main()
{
  fragColor = vec4(mixColor, opacity)*borderColor*uColor;
  fragColor.a *= smoothstep(0.0, 1.5, vAlpha)*smoothstep( borderSize+1.5, borderSize, vAlpha );
}

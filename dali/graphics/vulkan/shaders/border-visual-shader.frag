#version 430

layout( set = 1, binding = 0, std140 ) uniform FragData
{
  vec4 uColor;
  vec4 borderColor;
  vec3 mixColor;
  float opacity;
};

layout( location = 0 ) out vec4 fragColor;

void main()
{
  fragColor = vec4(mixColor, opacity)*borderColor*uColor;
}

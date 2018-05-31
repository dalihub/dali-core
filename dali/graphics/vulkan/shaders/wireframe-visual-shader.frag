#version 430

layout( set = 1, binding = 0, std140 ) uniform FragData
{
  vec4 uColor;
  vec3 mixColor;
};

layout( location = 0 ) out vec4 fragColor;

void main()
{
  fragColor = uColor * vec4( mixColor, 1.0 );
}

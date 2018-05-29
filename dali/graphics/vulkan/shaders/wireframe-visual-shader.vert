#version 430

// in variables
layout( location = 0 ) in vec2 aPosition;

layout( set = 0, binding = 0, std140 ) uniform VertexData
{
  mat4 uMvpMatrix;
  vec3 uSize;

  // Visual size and offset
  vec2 offset;
  vec2 size;
  vec4 offsetSizeMode;
  vec2 origin;
  vec2 anchorPoint;
};

vec4 ComputeVertexPosition()
{
  vec2 visualSize = mix(uSize.xy*size, size, offsetSizeMode.zw );
  vec2 visualOffset = mix( offset, offset/uSize.xy, offsetSizeMode.xy);
  return vec4( (aPosition + anchorPoint)*visualSize + (visualOffset + origin)*uSize.xy, 0.0, 1.0 );
}

void main()
{
  gl_Position = uMvpMatrix * ComputeVertexPosition();
}

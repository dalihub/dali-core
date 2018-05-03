#version 430

// in variables
layout( location = 0 ) in vec2 aPosition;
layout( location = 0 ) in vec2 aDrift;

// uniforms
layout( set = 0, binding = 0, std140 ) uniform VertexData
{
  mat4 uMvpMatrix;
  vec3 uSize;
  float borderSize;
};

// out variables
layout( location = 0 ) out float vAlpha;

void main()
{
  vec2 position = aPosition*(uSize.xy+vec2(0.75)) + aDrift*(borderSize+1.5);
  gl_Position = uMvpMatrix * vec4(position, 0.0, 1.0);
  vAlpha = min( abs(aDrift.x), abs(aDrift.y) )*(borderSize+1.5);
}

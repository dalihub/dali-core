#version 430

layout(location=0) in vec2 aPosition;
layout(location=0) out vec2 vTexCoord;

layout(set = 0, binding = 0, std140) uniform VertData
{
    mat4 uModelMatrix;
    mat4 uMvpMatrix;
    vec3 uSize;
    vec2 uFixed[ 3 ];
    vec2 uStretchTotal;


    //Visual size and offset
    vec2 offset;
    vec2 size;
    vec4 offsetSizeMode;
    vec2 origin;
    vec2 anchorPoint;
};

void main()
{
  vec2 visualSize = mix(uSize.xy*size, size, offsetSizeMode.zw );
  vec2 visualOffset = mix( offset, offset/uSize.xy, offsetSizeMode.xy);

  vec2 size         = visualSize.xy;

  vec2 fixedFactor  = vec2( uFixed[ int( ( aPosition.x + 1.0 ) * 0.5 ) ].x, uFixed[ int( ( aPosition.y  + 1.0 ) * 0.5 ) ].y );
  vec2 stretch      = floor( aPosition * 0.5 );
  vec2 fixedTotal   = uFixed[ 2 ];

  vec4 vertexPosition = vec4( fixedFactor + ( size - fixedTotal ) * stretch, 0.0, 1.0 );
  vertexPosition.xy -= size * vec2( 0.5, 0.5 );
  vertexPosition.xy =  vertexPosition.xy + anchorPoint*size + (visualOffset + origin)*uSize.xy;\

  vertexPosition = uMvpMatrix * vertexPosition;

  vTexCoord = ( fixedFactor + stretch * uStretchTotal ) / ( fixedTotal + uStretchTotal );

  gl_Position = vertexPosition;
}

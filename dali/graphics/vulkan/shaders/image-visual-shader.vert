#version 430

layout( location = 0 ) in vec2 aPosition;

layout( set = 0, binding = 0, std140 ) uniform vertData
{
    mat4 uModelMatrix;
    mat4 uViewMatrix;
    mat4 uProjection;
    vec3 uSize;
    vec4 pixelArea;
    float uPixelAligned;

    //Visual size and offset
    vec2 offset;
    vec2 size;
    vec4 offsetSizeMode;
    vec2 origin;
    vec2 anchorPoint;
};

layout( location = 0 ) out vec2 vTexCoord;

vec4 ComputeVertexPosition()
{
    vec2 visualSize = mix(uSize.xy*size, size, offsetSizeMode.zw );
    vec2 visualOffset = mix( offset, offset/uSize.xy, offsetSizeMode.xy);
    return vec4( (aPosition + anchorPoint)*visualSize + (visualOffset + origin)*uSize.xy, 0.0, 1.0 );
}

void main()
{
    vec4 vertexPosition = uViewMatrix * uModelMatrix * ComputeVertexPosition();
    vec4 alignedVertexPosition = vertexPosition;
    alignedVertexPosition.xy = floor ( vertexPosition.xy ); // Pixel alignment
    vertexPosition = uProjection * mix( vertexPosition, alignedVertexPosition, uPixelAligned );
    vTexCoord = pixelArea.xy+pixelArea.zw*(aPosition + vec2(0.5) );
    gl_Position = vertexPosition;
}
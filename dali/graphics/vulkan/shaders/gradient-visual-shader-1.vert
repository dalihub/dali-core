#version 430

layout(location=0) in vec2 aPosition;
layout(location=0) out vec2 vTexCoord;

layout(set = 0, binding = 0, std140) uniform VertData
{
    mat4 uMvpMatrix;
    vec3 uSize;
    mat3 uAlignmentMatrix;

    //Visual size and offset
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
    vec4 vertexPosition = vec4(aPosition, 0.0, 1.0);
    vertexPosition.xyz *= uSize;
    gl_Position = uMvpMatrix * ComputeVertexPosition();

    vTexCoord = (uAlignmentMatrix*vertexPosition.xyw).xy;
}

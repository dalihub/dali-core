#version 430

layout(location=0) in vec3 aPosition;
layout(location=1) in vec2 aTexCoord;
layout(location=2) in vec3 aNormal;
layout(location=3) in vec3 aTangent;
layout(location=4) in vec3 aBiNormal;

layout(location=0) out vec2 vTexCoord;
layout(location=1) out vec3 vLightDirection;
layout(location=2) out vec3 vHalfVector;

layout( set = 0, binding = 0, std140 ) uniform VertData
{
    vec3 uSize;
    mat4 uMvpMatrix;
    mat4 uModelView;
    mat4 uViewMatrix;
    mat3 uNormalMatrix;
    mat4 uObjectMatrix;
    vec3 lightPosition;
    vec2 uStageOffset;

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
    float scaleFactor = min( visualSize.x, visualSize.y );
    vec3 originFlipY =  vec3(origin.x, -origin.y, 0.0);
    vec3 anchorPointFlipY = vec3( anchorPoint.x, -anchorPoint.y, 0.0);
    vec3 offset = vec3( ( offset / uSize.xy ) * offsetSizeMode.xy + offset * (1.0-offsetSizeMode.xy), 0.0) * vec3(1.0,-1.0,1.0);
    return vec4( (aPosition + anchorPointFlipY)*scaleFactor + (offset + originFlipY)*uSize, 1.0 );
}

void main()
{
    vec4 normalisedVertexPosition = ComputeVertexPosition();
    vec4 vertexPosition = uObjectMatrix * normalisedVertexPosition;
    vertexPosition = uMvpMatrix * vertexPosition;

    vec4 mvVertexPosition = uModelView * normalisedVertexPosition;

    vec3 tangent = normalize( uNormalMatrix * mat3( uObjectMatrix ) * aTangent );
    vec3 binormal = normalize( uNormalMatrix * mat3( uObjectMatrix ) * aBiNormal );
    vec3 normal = normalize( uNormalMatrix * mat3( uObjectMatrix ) * aNormal );

    vec4 mvLightPosition = vec4( ( lightPosition.xy - uStageOffset ), lightPosition.z, 1.0 );
    mvLightPosition = uViewMatrix * mvLightPosition;
    vec3 vectorToLight = normalize( mvLightPosition.xyz - mvVertexPosition.xyz );
    vLightDirection.x = dot( vectorToLight, tangent );
    vLightDirection.y = dot( vectorToLight, binormal );
    vLightDirection.z = dot( vectorToLight, normal );

    vec3 viewDirection = normalize( -mvVertexPosition.xyz );
    vec3 halfVector = normalize( viewDirection + vectorToLight );
    vHalfVector.x = dot( halfVector, tangent );
    vHalfVector.y = dot( halfVector, binormal );
    vHalfVector.z = dot( halfVector, normal );

    vTexCoord = aTexCoord;
    gl_Position = vertexPosition;
}

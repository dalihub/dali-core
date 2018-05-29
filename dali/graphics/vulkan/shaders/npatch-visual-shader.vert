#version 430

layout(location=0) in vec2 aPosition;
layout(location=0) out vec2 vTexCoord;

// using specialization constants
layout(constant_id = 0) const int FACTOR_SIZE_X = 4;
layout(constant_id = 1) const int FACTOR_SIZE_Y = 4;

layout(set=0, binding=0, std140) uniform VertData
{
    mat4 uMvpMatrix;
    vec3 uSize;
    vec2 uNinePatchFactorsX[ FACTOR_SIZE_X ];
    vec2 uNinePatchFactorsY[ FACTOR_SIZE_Y ];

    //Visual size and offset
    vec2 offset;
    vec2 size;
    vec4 offsetSizeMode;
    vec2 origin;
    vec2 anchorPoint;
};

void main()
{
    vec2 fixedFactor  = vec2( uNinePatchFactorsX[ int( ( aPosition.x + 1.0 ) * 0.5 ) ].x, uNinePatchFactorsY[ int( ( aPosition.y + 1.0 ) * 0.5 ) ].x );
    vec2 stretch      = vec2( uNinePatchFactorsX[ int( ( aPosition.x       ) * 0.5 ) ].y, uNinePatchFactorsY[ int( ( aPosition.y       ) * 0.5 ) ].y );

    vec2 fixedTotal   = vec2( uNinePatchFactorsX[ FACTOR_SIZE_X - 1 ].x, uNinePatchFactorsY[ FACTOR_SIZE_Y - 1 ].x );
    vec2 stretchTotal = vec2( uNinePatchFactorsX[ FACTOR_SIZE_X - 1 ].y, uNinePatchFactorsY[ FACTOR_SIZE_Y - 1 ].y );


    vec2 visualSize = mix(uSize.xy*size, size, offsetSizeMode.zw );
    vec2 visualOffset = mix( offset, offset/uSize.xy, offsetSizeMode.xy);

    vec4 vertexPosition = vec4( ( fixedFactor + ( visualSize.xy - fixedTotal ) * stretch / stretchTotal ) +  anchorPoint*visualSize + (visualOffset + origin)*uSize.xy, 0.0, 1.0 );
    vertexPosition.xy -= visualSize.xy * vec2( 0.5, 0.5 );

    vertexPosition = uMvpMatrix * vertexPosition;

    vTexCoord = ( fixedFactor + stretch ) / ( fixedTotal + stretchTotal );

    gl_Position = vertexPosition;
}

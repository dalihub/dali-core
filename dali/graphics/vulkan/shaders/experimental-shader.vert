#version 430

layout( location = 0 ) in vec3 aPosition;
layout( location = 1 ) in vec3 aSize;

layout( set = 0, binding = 0, r32f ) uniform image2D myStorageImage;

layout( set = 2, binding = 0, std140 ) uniform world
{
    mat4 mvp;
    vec4 color;
    vec3 size;
};

layout( set = 2, binding = 1 ) uniform samplerCube uTexture;

layout( set = 2, binding = 2 ) uniform sampler2D uMask;
layout( set = 2, binding = 5, std140 ) uniform clipUniform
{
    mat4 clip;
};

layout( std430, set = 0, binding = 1 ) buffer myStorageBuffer
{
    mat4 myMat;
    vec4 myVec;
    vec4 bones[];
};

layout( location = 0 ) out vec4 triColor;

void main()
{
    gl_Position = clip * mvp * vec4( aPosition* size * aSize, 1.0 );
    //gl_Position = vec4( aPosition, 1.0 );
    triColor = color * texture( uTexture, aSize.xyz ) * texture( uMask, aSize.xy );
}
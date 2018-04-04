#version 430

layout( location = 0 ) in vec3 aPosition;

layout( set = 0, binding = 0, std140 ) uniform world
{
    mat4 mvp;
    vec4 color;
    vec3 size;
};

layout( set = 0, binding = 1, std140 ) uniform clipUniform
{
    mat4 clip;
};

layout( location = 0 ) out vec4 triColor;

layout( location = 1 ) out vec2 uvCoords;

void main()
{
    gl_Position = clip * mvp * vec4( aPosition* size, 1.0 );
    uvCoords = aPosition.xy + vec2( 0.5, 0.5 );
    triColor = color;
}
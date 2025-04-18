#version 440

layout (location = 0) in vec4 VertexPosition; //Set particular attribute number //Get the first index of VAO and set it on VertexPosition
layout (location = 1) in vec3 VertexNormal; //Get the second index of VAO and set it on VertexNormal
layout (location = 2) in vec2 VertexTexcoord; //Get the third index of VAO and set it on VertexTexcoord

out vec3 Position;
out vec3 Normal;
out vec2 Texcoord;

uniform mat4 ModelMatrix;
uniform mat3 NormalMatrix;
uniform mat4 MVP;

void main()
{
    Normal = normalize(NormalMatrix * VertexNormal);
    Position = (ModelMatrix * VertexPosition).xyz;
    Texcoord = VertexTexcoord;
    gl_Position = MVP * VertexPosition;
}

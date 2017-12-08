#version 330 core

layout(location = 0) in vec3 position; // Position of the vertex
layout(location = 1) in vec3 normal;   // Normal of the vertex
layout(location = 5) in vec2 texCoord; // UV texture coordinates

out vec3 FragPos;
out vec2 TexCoords;
out vec3 Normal;

uniform mat4 p;
uniform mat4 v;
uniform mat4 m;

uniform bool useLighting;     // Whether to calculate lighting using lighting equation
uniform bool useArrowOffsets; // True if rendering the arrowhead of a normal for Shapes

void main() {
    vec4 viewPos = v * m * vec4(position, 1.0f);
    FragPos = viewPos.xyz;
    gl_Position = p * viewPos;
    TexCoords = texCoord;

    mat3 normalMatrix = transpose(inverse(mat3(v * m)));
    Normal = normalMatrix * normal;
}

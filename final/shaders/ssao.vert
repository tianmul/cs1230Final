#version 330 core
layout (location = 6) in vec2 position;
layout (location = 5) in vec2 texCoord;

out vec2 TexCoords;

void main()
{
    gl_Position = vec4(vec3(position,0.0f), 1.0f);
    TexCoords = texCoord;
}

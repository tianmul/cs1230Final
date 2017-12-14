#version 330 core
layout (location = 0) in vec3 position;

// Light data
const int MAX_LIGHTS = 10;
uniform int lightTypes[MAX_LIGHTS];         // 0 for point, 1 for directional
uniform vec3 lightPositions[MAX_LIGHTS];    // For point lights
uniform vec3 lightDirections[MAX_LIGHTS];   // For directional lights
//uniform vec3 lightAttenuations[MAX_LIGHTS]; // Constant, linear, and quadratic term
uniform vec3 lightColors[MAX_LIGHTS];

uniform vec3 ambient_color;
uniform vec3 diffuse_color;
uniform vec3 specular_color;
uniform float shininess;

uniform mat4 m;
uniform int lightIndex;
uniform mat4 lightSpaceMatrix[MAX_LIGHTS];
void main() {
    gl_Position = lightSpaceMatrix[lightIndex] * m * vec4(position, 1.0f);
}

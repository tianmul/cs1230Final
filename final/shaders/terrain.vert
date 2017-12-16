#version 400 core

layout(location = 0) in vec3 OS_position;
layout(location = 1) in vec3 OS_normal;
layout(location = 5) in vec3 texCoord;
layout(location = 2) in vec3 OS_color;

uniform mat4 model, view, projection;


// Light data
const int MAX_LIGHTS = 10;
uniform int lightTypes[MAX_LIGHTS];         // 0 for point, 1 for directional
uniform vec3 lightPositions[MAX_LIGHTS];    // For point lights
uniform vec3 lightDirections[MAX_LIGHTS];   // For directional lights
//uniform vec3 lightAttenuations[MAX_LIGHTS]; // Constant, linear, and quadratic term
uniform vec3 lightColors[MAX_LIGHTS];


uniform bool useLighting;     // Whether to calculate lighting using lighting equation
uniform bool useArrowOffsets; // True if rendering the arrowhead of a normal for Shapes

out vec3 color;
out vec2 uv;

void main() {
    uv = texCoord.xy;
    vec3 WS_position = (model * vec4(OS_position, 1.0)).xyz;
    vec3 WS_normal = (model * vec4(OS_normal, 0.0)).xyz;
    gl_Position = projection * view * vec4(WS_position, 1.0);

    vec4 position_cameraSpace = view * model * vec4(OS_position, 1.0);
    vec4 normal_cameraSpace = vec4(normalize(mat3(transpose(inverse(view * model))) * OS_normal), 0);
    vec3 diffuse_color = vec3(0.8);

        if (useLighting) {
            color = vec3(0);
        for (int i = 0; i < MAX_LIGHTS; i++) {
                vec4 vertexToLight = vec4(0);
                // Point Light
                if (lightTypes[i] == 0) {
                    vertexToLight = normalize(view * vec4(lightPositions[i], 1) - position_cameraSpace);
                } else if (lightTypes[i] == 1) {
                    // Dir Light
                    vertexToLight = normalize(view * vec4(-lightDirections[i], 0));
                }

                // Add diffuse component
                float diffuseIntensity = max(0.0, dot(vertexToLight, normal_cameraSpace));
                if (diffuseIntensity > 0.95)
                        color = max(vec3(0), lightColors[i] * diffuse_color * 1.1);
                else if (diffuseIntensity > 0.5)
                        color = max(vec3(0), lightColors[i] * diffuse_color * 0.85);
                else if (diffuseIntensity > 0.25)
                        color = max(vec3(0), lightColors[i] * diffuse_color * 0.7);
                else
                        color = max(vec3(0), lightColors[i] * diffuse_color  * 0.6);

            }
        } else {
            color = vec3(1);
        }
}

#version 330 core
layout (location = 0) in vec3 position;
layout(location = 1) in vec3 normal;   // Normal of the vertex
layout (location = 5) in vec2 texCoord;
layout (location = 6) in vec2 positionsquare;
layout(location = 10) in float arrowOffset; // Sideways offset for billboarded normal arrows

uniform int square;
out vec2 TexCoords;
out vec3 color; // Computed color for this vertex
out vec2 uv;

// Transformation matrices
uniform mat4 p;
uniform mat4 v;
uniform mat4 m;

// Light data
const int MAX_LIGHTS = 10;
uniform int lightTypes[MAX_LIGHTS];         // 0 for point, 1 for directional
uniform vec3 lightPositions[MAX_LIGHTS];    // For point lights
uniform vec3 lightDirections[MAX_LIGHTS];   // For directional lights
//uniform vec3 lightAttenuations[MAX_LIGHTS]; // Constant, linear, and quadratic term
uniform vec3 lightColors[MAX_LIGHTS];

// Material data
uniform vec3 ambient_color;
uniform vec3 diffuse_color;
uniform vec3 specular_color;
uniform float shininess;
uniform vec2 repeatUV;

uniform bool useLighting;     // Whether to calculate lighting using lighting equation
uniform bool useArrowOffsets; // True if rendering the arrowhead of a normal for Shapes

uniform bool usingShadowMap;
uniform sampler2D depthMap[MAX_LIGHTS];
uniform mat4 lightSpaceMatrix[MAX_LIGHTS];

float ShadowCalculation(int i, vec4 fragPosLightSpace, vec4 normal_worldSpace, vec4 position_worldSpace) {
    if (!usingShadowMap) return 0.0f;

    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(depthMap[i], projCoords.xy).r;
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    // calculate bias (based on depth map resolution and slope)
    vec3 normal = normalize(normal_worldSpace.xyz);
    vec3 lightDir = normalize(lightPositions[0] - position_worldSpace.xyz);
    float bias = max(0.01 * (1.0 - dot(normal, lightDir)), 0.001);
    // check whether current frag pos is in shadow

    // PCF
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(depthMap[i], 0);
    for(int x = -1; x <= 1; ++x) {
        for(int y = -1; y <= 1; ++y) {
            float pcfDepth = texture(depthMap[i], projCoords.xy + vec2(x, y) * texelSize).r;
            shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;
        }
    }
    shadow /= 9.0;

    // keep the shadow at 0.0 when outside the far_plane region of the light's frustum.
    if(projCoords.z > 1.0)
        shadow = 0.0;

    return shadow;
//    return closestDepth;
}
void main()
{
    uv = texCoord.xy;
    if(square == 1){
        gl_Position = vec4(vec3(positionsquare,0.0f), 1.0f);
        TexCoords = vec2((gl_Position.x+1.0f)/2.0f, (1.0f+gl_Position.y)/2.0f);
        //TexCoords = texCoord;
        color = vec3(0.0f);
    }
    else{
        gl_Position = vec4(0.0f);
        TexCoords = vec2(0.0f);


        vec4 position_cameraSpace = v * m * vec4(position, 1.0);
        vec4 normal_cameraSpace = vec4(normalize(mat3(transpose(inverse(v * m))) * normal), 0);

        vec4 position_worldSpace = m * vec4(position, 1.0);
        vec4 normal_worldSpace = vec4(normalize(mat3(transpose(inverse(m))) * normal), 0);

        if (useArrowOffsets) {
            // Figure out the axis to use in order for the triangle to be billboarded correctly
            vec3 offsetAxis = normalize(cross(vec3(position_cameraSpace), vec3(normal_cameraSpace)));
            position_cameraSpace += arrowOffset * vec4(offsetAxis, 0);
        }

        gl_Position = p * position_cameraSpace;
        TexCoords = vec2(gl_Position.x*5.0f+0.5f, 1.0f+gl_Position.y*5.0f+0.5f);
        if (useLighting) {
            color = ambient_color.xyz; // Add ambient component



            //color = ambient_color.xyz; // Add ambient component
            for (int i = 0; i < MAX_LIGHTS; i++) {
                vec4 lightSpacePosition = lightSpaceMatrix[i] * position_worldSpace;
                float shadow = ShadowCalculation(i, lightSpacePosition, normal_worldSpace, position_worldSpace);

                vec4 vertexToLight = vec4(0);
                // Point Light
                if (lightTypes[i] == 0) {
                    vertexToLight = normalize(v * vec4(lightPositions[i], 1) - position_cameraSpace);
                } else if (lightTypes[i] == 1) {
                    // Dir Light
                    vertexToLight = normalize(v * vec4(-lightDirections[i], 0));
                }

                // Add diffuse component
                float diffuseIntensity = max(0.0, dot(vertexToLight, normal_cameraSpace));
                color += max(vec3(0), lightColors[i] * diffuse_color * diffuseIntensity) * (1.0f-shadow);

                // Add specular component
                vec4 lightReflection = normalize(-reflect(vertexToLight, normal_cameraSpace));
                vec4 eyeDirection = normalize(vec4(0,0,0,1) - position_cameraSpace);
                float specIntensity = pow(max(0.0, dot(eyeDirection, lightReflection)), shininess);
                color += max (vec3(0), lightColors[i] * specular_color * specIntensity) * (1.0f-shadow);

    //            color = vec3(shadow);
            }
            //vec2 temp = vec2((position_cameraSpace.x+1.0f)/2.0f, (position_cameraSpace.y+1.0f)/2.0f);
            //color = vec3(texture(ssao, position_cameraSpace.xy).r);
            //TexCoords = vec2((temp.x+1.0f)/2.0f*8.0f, (temp.y+1.0f)/2.0f*6.0f);
        }

            else {
            color = ambient_color + diffuse_color;
        }
        color = clamp(color, 0.0, 1.0);

    }
}



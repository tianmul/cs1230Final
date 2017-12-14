#version 330 core
out vec4 FragColor;
in vec2 texc;
in vec3 color;

uniform sampler2D gPositionDepth;
uniform sampler2D gNormal;
uniform sampler2D gAlbedo;
uniform sampler2D ssao;
uniform int square;
uniform vec2 SCR_SIZE;
uniform bool usingSSAO;

void main()
{
    float AmbientOcclusion =texture(ssao, vec2(gl_FragCoord.x / SCR_SIZE.x, gl_FragCoord.y / SCR_SIZE.y)).r;

    if (square == 1) {
        FragColor = vec4(vec3(1.0f-AmbientOcclusion), 1.0);
    }
    else{
        //vec3 texColor = texture(tex, texc).rgb;
        //texColor = clamp(texColor + vec3(1-useTexture), vec3(0), vec3(1));
        //fragColor = vec4(color * texColor, 1);

        if(!usingSSAO) FragColor = vec4(color,1);
        else FragColor = vec4(color*AmbientOcclusion,1);
        //FragColor = vec4(vec3(1.0f-AmbientOcclusion), 1.0);

    }
}

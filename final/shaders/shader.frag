#version 330 core
out vec4 FragColor;
in vec3 color;
in vec2 uv;
in vec3 ambientColor;

uniform sampler2D gPositionDepth;
uniform sampler2D gNormal;
uniform sampler2D gAlbedo;
uniform sampler2D ssao;
uniform sampler2D mysp;
uniform int square;
uniform vec2 SCR_SIZE;
uniform bool usingSSAO;
uniform bool usingTexutre;
vec4 texColor;

void main()
{
    float AmbientOcclusion =texture(ssao, vec2(gl_FragCoord.x / SCR_SIZE.x, gl_FragCoord.y / SCR_SIZE.y)).r;

    if (square == 1) {
        FragColor = vec4(vec3(1.0f-AmbientOcclusion), 1.0);
    }
    else{
        FragColor = vec4(ambientColor*AmbientOcclusion, 0.0);
        //vec3 texColor = texture(tex, texc).rgb;
        //texColor = clamp(texColor + vec3(1-useTexture), vec3(0), vec3(1));
        //fragColor = vec4(color * texColor, 1);
        if(usingTexutre)
        {
            texColor = texture(mysp, uv);
            if(usingSSAO)
                FragColor += vec4(  color * clamp(texColor.xyz,0,1) ,1);
            else
                FragColor = vec4(  color * clamp(texColor.xyz,0,1) ,1);
        } else {
            if(!usingSSAO) FragColor = vec4(color,1);
            else FragColor += vec4(color,1);
        }

//        texColor = texture(mysp, uv);
//        FragColor = vec4( texColor.xyz,1);

        //FragColor = vec4(vec3(1.0f-AmbientOcclusion), 1.0);


    }
}

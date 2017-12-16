#version 400 core

in vec2 uv;
in vec3 color;

out vec4 fragColor;
uniform sampler2D mysp;
vec4 texColor;
void main(){

    texColor = texture(mysp, uv);
    fragColor = vec4(color*texColor.xyz,1);
    //vec3 WS_toLight = normalize(vec3(10.0) - WS_position);
    //fragColor = vec3(0.3 *  texColor.xyz +  texColor.xyz * max(0.0, dot(normalize(WS_normal), WS_toLight)));

}

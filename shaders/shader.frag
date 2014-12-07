#version 330 core

in vec3 color;
out vec4 fragColor;

void main(){
//    vec3 texColor = texture(tex, texc).rgb;
//    texColor = clamp(texColor + vec3(1-useTexture), vec3(0), vec3(1));
//    fragColor = vec4(color * texColor, 1);
    fragColor = vec4(1, 0, 0, 1);
//    fragColor = vec4(vec3(1-useTexture), 1);
//    fragColor = vec4(1,1,1,1);
//    fragColor = vec4(useTexture, 0, 0, 1);
//    fragColor = vec4(color, 1);
}

#version 330 core

in vec3 color;
in vec4 pos_shadowSpace;
uniform sampler2D tex;
out vec4 fragColor;

void main(){

    float depthVal = pos_shadowSpace.z; // Z of the current object in sun-space
    float shadowVal = texture(tex, pos_shadowSpace.xy).z; // Z of the nearest thing
    fragColor = vec4(depthVal, depthVal, depthVal, 1);

    fragColor = vec4(color,1 );

}

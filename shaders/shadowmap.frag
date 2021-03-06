#version 330 core

in vec4 pos_shadowSpace;
//uniform sampler2D tex;
out vec4 fragColor;

void main(){
    // Adjustment
    float depth2 = (2.0 * gl_FragCoord.z - gl_DepthRange.near - gl_DepthRange.far) /
        (gl_DepthRange.far - gl_DepthRange.near);
    float depth3 = pos_shadowSpace.z / pos_shadowSpace.w; // Z of the current object in sun-space
    // hack hack hack
    float depth4 = (depth3 - 0.999) * 1000;
//    fragColor = vec4(depth2, depth2, depth2, 1.0);
//    fragColor = vec4(depth, depth, depth, 1.0);
//    fragColor = vec4(depth3, depth3, depth3, 1.0);
    fragColor = vec4(depth4, depth4, depth4, 1.0);
//    fragColor = vec4(1, 0, 0, 1.0);
}

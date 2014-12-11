#version 330 core

in float depth;
//uniform sampler2D tex;
out vec4 fragColor;

void main(){
    // Adjustment
    float depth2 = (2.0 * gl_FragCoord.z - gl_DepthRange.near - gl_DepthRange.far) /
        (gl_DepthRange.far - gl_DepthRange.near);
    fragColor = vec4(depth2, depth2, depth2, 1.0);
    fragColor = vec4(depth, 0.25, depth, 1.0);

//    float asdf = gl_FragCoord.z / 11.0;
//    fragColor = vec4(asdf, d, depth, 1.0);
}

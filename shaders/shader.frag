#version 330 core

in vec4 pos_shadowSpace;
uniform sampler2D tex;
out vec4 fragColor;

void main(){
    //depths in the 0.54- 0.63 range (for the first thing)
    //x/y are a little bit higher than 0.5 (0.51 ish)

    float depthVal = pos_shadowSpace.z / pos_shadowSpace.w;//11.0; // Z of the current object in sun-space
    vec2 adj = vec2((pos_shadowSpace.x / pos_shadowSpace.w + 1) / 2,
                    (pos_shadowSpace.y / pos_shadowSpace.w + 1) / 2);
    float shadowVal = texture(tex, adj).x;
//    float altShadowVal = texture(tex, adj).z;

//    fragColor = vec4(texture(tex, pos_shadowSpace.xy).xyz, 1);

    float diff = depthVal - shadowVal; // gap between the two. + = difference

    // Point not in the light.
    if (adj.x <= 0 || adj.x >= 1
            || adj.y <= 0 || adj.y >= 1) {
        fragColor = vec4(1, 0, 0 ,1);
    }
    // Something fucked up
    else if (depthVal < 0 || depthVal > 1
             || shadowVal < 0 || shadowVal > 1) {
        fragColor = vec4(1, 1, 0 ,1);
    }
    else {
        if (diff > 0.01) {
            fragColor = vec4(0, 1, 0, 1);
        }
        else if (depthVal == 1) {
            fragColor = vec4(1, 1, 1, 1);
        }
        else {
            fragColor = vec4(0, 0, 1, 1);
        }
//        fragColor = vec4(depthVal, depthVal, depthVal, 1);
//        fragColor = vec4(pos_shadowSpace.xyz, 1);
//        fragColor = vec4(shadowVal, shadowVal, shadowVal, 1);
//        fragColor = vec4(altShadowVal, altShadowVal, altShadowVal, 1);
    }
}

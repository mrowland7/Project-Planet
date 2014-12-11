#version 330 core

in vec3 color;
in vec4 pos_shadowSpace;
in vec4 pos_modelSpace;
uniform sampler2D tex;
out vec4 fragColor;

void main(){

    //MAX's CODE
    fragColor = vec4(color,1 );















/*
    //Mike's Code

    //depths in the 0.54- 0.63 range (for the first thing)
    //x/y are a little bit higher than 0.5 (0.51 ish)
    float depthVal = pos_shadowSpace.z; // Z of the current object in sun-space
    float shadowVal = texture(tex, pos_shadowSpace.xy).z; // Z of the nearest thing
    fragColor = vec4(depthVal, depthVal, depthVal, 1);

    // HOW TO GET TEXTURE FROM SHADOWMAP?

    float depthVal = pos_shadowSpace.z;// / 11.0; // Z of the current object in sun-space
    float shadowVal = texture(tex, pos_shadowSpace.xy).x;
    float sanity = texture(tex, pos_shadowSpace.xy).g;

    float diff = depthVal - shadowVal; // gap between the two. + = difference

    if (shadowVal > 0 && shadowVal < 1) {
        fragColor = vec4(0, 1, 0 ,1);
    }
    else {
        fragColor = vec4(pos_shadowSpace.xyz, 1);
    }
//    else if (diff != 0) {
//        fragColor = vec4(shadowVal, shadowVal, shadowVal, 1);
//    }
//    else {
//        fragColor = vec4(0, 1, 0 ,1);
//    }*/
}

#version 330 core

in vec3 normalWorldSpace;
in vec3 vertexToLight;
in vec3 _lightColor;


in vec3 color;
in vec4 pos_shadowSpace;
in vec4 pos_modelSpace;
uniform sampler2D tex;
out vec4 fragColor;

void main(){

    //MAX's CODE

    //LIGHTING
    vec3 _color = vec3(1,1,1);
    // Add diffuse component
    vec3 ambient = _color*.2f;
    float diffuseIntensity = clamp(.7*dot(vertexToLight, normalWorldSpace),0,1);
    vec3 diffuse = max(vec3(0), _lightColor * _color * diffuseIntensity);

    // Add specular component
    //vec4 lightReflection = normalize(-reflect(vertexToLight, normal_cameraSpace));
    //vec4 eyeDirection = normalize(vec4(0,0,0,1) - position_cameraSpace);
    //float specIntensity = pow(max(0.0, dot(eyeDirection, lightReflection)), shininess);
    //color += max (vec3(0), lightColors[i] * specular_color * specIntensity);

    fragColor = vec4(ambient+ diffuse,1 );















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

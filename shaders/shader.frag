#version 330 core

in vec3 normalWorldSpace;
in vec3 vertexToLight;
in vec3 _lightColor;

in vec2 coord; // terrain mapping coord
in float height;
in float biome; // first height, second biome

uniform int shadowsOn;

in vec3 color;
in vec4 pos_shadowSpace;
uniform sampler2D tex;
out vec4 fragColor;

uniform sampler2D snowTexture;
uniform sampler2D rockTexture;
uniform sampler2D lavaTexture;
uniform sampler2D dirtTexture;

const vec2 lavaRange = vec2(-0.5, -0.25);
const vec2 dirtRange = vec2(-0.3, -0.1);
const vec2 rockRange = vec2(-0.25, 0.05);
const vec2 snowRange = vec2(0.05, 0.17);

float regionWeight(vec2 region) {
    float regionDiff = region.y - region.x;
    float weight = (regionDiff - (abs(height - region.y))) / regionDiff;
    return max(0.0, weight);
}
vec4 sampleTextures()
{
    vec4 dirt = texture(dirtTexture, coord) * regionWeight(dirtRange);
    vec4 lava = texture(lavaTexture, coord) * regionWeight(lavaRange);
    vec4 rock = texture(rockTexture, coord) * regionWeight(rockRange);
    vec4 snow = texture(snowTexture, coord) * regionWeight(snowRange);
    return dirt + lava + rock + snow;// + vec4(0.5, 0.5, 0, 1);

}
vec2 rotate(vec2 blah)
{
    // internet says this is a RNG: http://stackoverflow.com/questions/12964279/
    vec2 seed = vec2(gl_FragCoord.y/gl_FragCoord.w, gl_FragCoord.z/gl_FragCoord.x);
    float randomish = 2 * 3.1415926535 * fract(sin(dot(gl_FragCoord.yz ,vec2(12.9898,78.233))) * 43758.5453);
    float cr = cos(randomish);
    float sr = sin(randomish);
    return vec2(
                blah.x * cr - blah.y * sr,
                blah.x * sr + blah.y * cr
                );
}

void main()
{
    //LIGHTING
    // Add diffuse component
    vec3 ambient = color*.1f;
    float diffuseIntensity = clamp(.9*dot(vertexToLight, normalWorldSpace),0,1);
    vec3 diffuse = max(vec3(0), _lightColor * color * diffuseIntensity);

    // Add specular component
    //vec4 lightReflection = normalize(-reflect(vertexToLight, normal_cameraSpace));
    //vec4 eyeDirection = normalize(vec4(0,0,0,1) - position_cameraSpace);
    //float specIntensity = pow(max(0.0, dot(eyeDirection, lightReflection)), shininess);
    //color += max (vec3(0), lightColors[i] * specular_color * specIntensity);

    vec4 planetTexture = sampleTextures();
    vec3 realColor = planetTexture.xyz + ambient + diffuse/3;//color + ambient + diffuse;

    float depthValUnadjusted = pos_shadowSpace.z / pos_shadowSpace.w;//11.0; // Z of the current object in sun-space
    float depthVal = (depthValUnadjusted - 0.999) * 1000; // hack hack hack
    vec2 adj = vec2((pos_shadowSpace.x / pos_shadowSpace.w + 1) / 2,
                    (pos_shadowSpace.y / pos_shadowSpace.w + 1) / 2);
    float shadowVal = texture(tex, adj).x;

    float diff = depthVal - shadowVal; // gap between the two. + = difference

    // Red: Point not in the light.
    if (adj.x <= 0 || adj.x >= 1
            || adj.y <= 0 || adj.y >= 1) {
        fragColor = vec4(1, 0, 0 ,1);
    }
    else {
        float visibility = 1.0;
        float bias = 0.02;
        if (shadowsOn == 2) {
            float sampleSpread = 200;
            float dropPer = 0.15/2/2;
            vec2 rotatedSamples[8] = vec2[] (
                    rotate(vec2(-.8, .1)) / sampleSpread,
                    rotate(vec2(-.2, -.8)) / sampleSpread,
                    rotate(vec2(.25, .75)) / sampleSpread,
                    rotate(vec2(.87, -.12)) / sampleSpread,
                    rotate(vec2(-.5, .4)) / sampleSpread,
                    rotate(vec2(.05, -.9)) / sampleSpread,
                    rotate(vec2(.42, .67)) / sampleSpread,
                    rotate(vec2(.68, -.42)) / sampleSpread
                    );
            for (int i = 0; i < 8; i++) {
                float val = texture(tex, adj + rotatedSamples[i]).x;
                float diff2 = depthVal - val;
                if (diff2 > bias) {
                    visibility = visibility - dropPer;
                }
            }
            vec2 gridSamples[9] = vec2[] (
                    rotate(vec2(-1, -1)) / sampleSpread,
                    rotate(vec2(-1, 0)) / sampleSpread,
                    rotate(vec2(-1, 1)) / sampleSpread,
                    rotate(vec2(0, -1)) / sampleSpread,
                    rotate(vec2(0, 0)) / sampleSpread,
                    rotate(vec2(0, 1)) / sampleSpread,
                    rotate(vec2(1, -1)) / sampleSpread,
                    rotate(vec2(1, 0)) / sampleSpread,
                    rotate(vec2(1, 1)) / sampleSpread
                    );
            for (int i = 0; i < 9; i++) {
                float val = texture(tex, adj + gridSamples[i]).x;
                float diff2 = depthVal - val;
                if (diff2 > bias) {
                    visibility = visibility - dropPer;
                }
            }
            if (visibility > 0.75) visibility = 1.0;
        }
        fragColor = vec4(visibility * realColor, 1.0);
    }
//    if (shadowsOn == 2) {
//        fragColor = vec4(0,1,0,1);
//    } else if (shadowsOn == 1) {
//        fragColor = vec4(0,1,1,1);
//    } else {
//        fragColor = vec4(1,0,0,1);
//    }

}


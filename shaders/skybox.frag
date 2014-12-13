#version 330 core

out vec4 fragColor;
uniform sampler2D stars;
//uniform samplerCube starsASDF;
uniform vec2 screenSize;
in vec3 look;

void main() {
    vec2 hack = vec2(
                gl_FragCoord.x / screenSize.x,
                gl_FragCoord.y / screenSize.y);
    vec4 star = texture(stars, hack.xy);
//    vec4 star = texture(starsASDF, normalize(look));
//    vec4 star = texture(starsASDF, look);
//    fragColor = vec4(1, .3, .7, 1.0);
    fragColor = vec4(look, 1.0);
//    fragColor = vec4(hack, star.z, 1);
    fragColor = vec4(star.xyz, 1.0);

    //plan: look is good, use it to sample things better
}

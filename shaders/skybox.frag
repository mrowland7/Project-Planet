#version 330 core

out vec4 fragColor;
uniform sampler2D stars;
uniform vec2 screenSize;

void main() {
    vec2 hack = vec2(
                gl_FragCoord.x / screenSize.x,
                gl_FragCoord.y / screenSize.y);
    vec4 star = texture(stars, hack.xy);
//    fragColor = vec4(1, .3, .7, 1.0);
    fragColor = star;
}

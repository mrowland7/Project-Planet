#version 330 core

uniform mat4 p;
uniform mat4 v;
uniform mat4 m;
uniform vec3 cameraEyePos;
in vec3 position;
//out vec3 pos;

void main() {
    vec3 relativePos = position - vec3(1,1,1); //- cameraEyePos;
    vec4 position_cameraSpace = v * m * vec4(relativePos, 1.0);
    gl_Position = p * position_cameraSpace;
//    gl_Position = relativePos;
    gl_Position  = vec4(position.xy, 0.99999 ,1);
//    pos = position;
}

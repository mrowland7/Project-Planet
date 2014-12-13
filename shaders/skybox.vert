#version 330 core

uniform mat4 p;
uniform mat4 v;
uniform mat4 m;
uniform vec3 cameraEyePos;
in vec3 position;
//out vec3 look;

void main() {
//    mat4x4 inverseP = inverse(p);
//    mat3 invMv = transpose(mat3(v * m));
//    vec4 unproj = inverseP * vec4(position, 1.0);
//    look = invMv * unproj.xyz;
    gl_Position  = vec4(position.xy, 0.99999 ,1);
}

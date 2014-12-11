#version 330 core
//    // far plane = 11, near = 1

out float depth;
out vec4 pos_shadowSpace;

// Transformation matrices
uniform mat4 p;
uniform mat4 v;
uniform mat4 m;
in vec3 position; // Position of the vertex
uniform vec3 lightPosition;
uniform vec3 lightColor;
uniform mat4 shadow_v; // Modelview Projection matrix. This maps the vertices in model (object) space to world coordinates

void main(){
    vec4 position_cameraSpace = v * m * vec4(position, 1.0);
//    depth = (-1 * position_cameraSpace.z) / 11.0;

    gl_Position = p * position_cameraSpace;
    depth = gl_Position.z;
    pos_shadowSpace = shadow_v * vec4(position, 1.0);
}

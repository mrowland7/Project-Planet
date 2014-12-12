#version 330 core

//out float depth;
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

    gl_Position = p * position_cameraSpace;
    pos_shadowSpace = shadow_v * vec4(position, 1.0);
}

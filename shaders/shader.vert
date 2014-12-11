#version 330 core

out vec3 color; // Computed color for this vertex
out vec4 pos_shadowSpace;
out vec4 pos_modelSpace;

// Transformation matrices
uniform mat4 p;
uniform mat4 v;
uniform mat4 m;
in vec3 position; // Position of the vertex
in vec3 normal;   // Normal of the vertex
uniform mat4 shadow_v; // Modelview Projection matrix. This maps the vertices in model (object) space to world coordinates

void main(){
    vec4 position_cameraSpace = v * m * vec4(position, 1.0);
    vec4 position_worldSpace = m * vec4(position, 1.0);

    pos_shadowSpace = shadow_v * vec4(position, 1.0);
//    // go from [-1, 1] to [0,1]
//    pos_shadowSpace = 0.5 * (pos_shadowSpace + vec4(1, 1, 1, 1));
    pos_modelSpace = vec4(position, 0);

    gl_Position = p * position_cameraSpace;
    color = vec3(0, 1.0, 0);//clamp(color, 0.0, 1.0) * allBlack;
}

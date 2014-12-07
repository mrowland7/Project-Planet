#version 330 core

out vec3 color; // Computed color for this vertex

// Transformation matrices
uniform mat4 p;
uniform mat4 v;
uniform mat4 m;
in vec3 position; // Position of the vertex
in vec3 normal;   // Normal of the vertex
uniform mat4 mvp; // Modelview Projection matrix. This maps the vertices in model (object) space to world coordinates

// Light data
//const int MAX_LIGHTS = 10;
//uniform int lightTypes[MAX_LIGHTS];         // 0 for point, 1 for directional
//uniform vec3 lightPositions[MAX_LIGHTS];    // For point lights
//uniform vec3 lightDirections[MAX_LIGHTS];   // For directional lights
//uniform vec3 lightAttenuations[MAX_LIGHTS]; // Constant, linear, and quadratic term
//uniform vec3 lightColors[MAX_LIGHTS];

void main(){
    gl_Position = mvp * vec4(position.xyz, 1.0);
    color = vec3(0, 1.0, 0);//clamp(color, 0.0, 1.0) * allBlack;
}

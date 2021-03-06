#version 330 core
out vec3 normalWorldSpace;
out vec3 vertexToLight;
out vec3 _lightColor;

out vec4 pos_shadowSpace;
out vec3 color;

out vec2 coord;
out float height;
out float biome;


// Transformation matrices
uniform mat4 p;
uniform mat4 v;
uniform mat4 m;
in vec3 position; // Position of the vertex
in vec3 normal;   // Normal of the vertex
in vec2 texCoord;
in vec2 data;
uniform mat4 mvp; // Modelview Projection matrix. This maps the vertices in model (object) space to world coordinates
uniform mat4 shadow_mvp; // Modelview Projection matrix. This maps the vertices in model (object) space to world coordinates
uniform vec3 objColor;

//Light data
uniform vec3 lightPosition;
uniform vec3 lightColor;
uniform mat4 shadow_v; // Modelview Projection matrix. This maps the vertices in model (object) space to world coordinates

void main(){

    vec4 position_cameraSpace = v * m * vec4(position, 1.0);
    //vec4 normal_cameraSpace = vec4(normalize(mat3(transpose(inverse(v * m))) * normal), 0);

    vec4 position_worldSpace = m * vec4(position, 1.0);
    vertexToLight = normalize(lightPosition - vec3(position_worldSpace));
    normalWorldSpace = normalize(mat3(transpose(inverse(m))) * normal);
    _lightColor = lightColor;

    pos_shadowSpace = shadow_v * vec4(position, 1.0);

    gl_Position = p * position_cameraSpace;
    color = objColor;

    coord = texCoord;
    height = data.x;
    biome = data.y;
}

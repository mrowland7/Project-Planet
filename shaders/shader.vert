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
uniform mat4 mvp; // Modelview Projection matrix. This maps the vertices in model (object) space to world coordinates
uniform mat4 shadow_mvp; // Modelview Projection matrix. This maps the vertices in model (object) space to world coordinates

//Light data
uniform vec3 lightPosition;
uniform vec3 lightColor;
uniform mat4 shadow_v; // Modelview Projection matrix. This maps the vertices in model (object) space to world coordinates

void main(){
    vec3 colorVal = vec3(1,1,1);

    vec4 position_cameraSpace = v * m * vec4(position, 1.0);
    //vec4 normal_cameraSpace = vec4(normalize(mat3(transpose(inverse(v * m))) * normal), 0);

    vec4 position_worldSpace = m * vec4(position, 1.0);
    vec4 normal_worldSpace = vec4(normalize(mat3(transpose(inverse(m))) * normal), 0);

    color = colorVal*.3;

    //LIGHTING
    vec4 vertexToLight = normalize(vec4(lightPosition,1) - position_worldSpace); //in world space

    // Add diffuse component
    float diffuseIntensity = clamp(.7*dot(vertexToLight, normal_worldSpace),0,1);
    color += max(vec3(0), lightColor * colorVal * diffuseIntensity);

    // Add specular component
    //vec4 lightReflection = normalize(-reflect(vertexToLight, normal_cameraSpace));
    //vec4 eyeDirection = normalize(vec4(0,0,0,1) - position_cameraSpace);
    //float specIntensity = pow(max(0.0, dot(eyeDirection, lightReflection)), shininess);
    //color += max (vec3(0), lightColors[i] * specular_color * specIntensity);




    pos_shadowSpace = shadow_v * vec4(position, 1.0);
//    // go from [-1, 1] to [0,1]
//    pos_shadowSpace = 0.5 * (pos_shadowSpace + vec4(1, 1, 1, 1));
    pos_modelSpace = vec4(position, 0);

    gl_Position = p * position_cameraSpace;
    //clamp(color, 0.0, 1.0) * allBlack;

    //color = vec3(normal);



}

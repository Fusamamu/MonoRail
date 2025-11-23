#shader vertex
#version 330 core
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texCoord;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoord;
out vec4 FragPosLightSpace;

layout(std140) uniform CameraData
{
    mat4 proj;
    mat4 view;
    vec3 view_position;
    float pad1; // padding for std140
};

layout(std140) uniform DirectionalLightBlock
{
    vec3 direction; float pad1;
    vec3 ambient;   float pad2;
    vec3 diffuse;   float pad3;
    vec3 specular;  float pad4;
    mat4 light_space_matrix;
} dirLight;

uniform mat4 model;

void main()
{
    vec4 worldPos = model * vec4(position, 1.0);
    FragPos  = worldPos.xyz;
    Normal   = mat3(transpose(inverse(model))) * normal;
    TexCoord = texCoord;

    FragPosLightSpace = dirLight.light_space_matrix * worldPos;

    gl_Position = proj * view * vec4(FragPos, 1.0);
}

#shader fragment
#version 330 core
layout(location = 0) out vec4 gPosition;
layout(location = 1) out vec4 gNormal;
layout(location = 2) out vec4 gAlbedo;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;
in vec4 FragPosLightSpace;

layout(std140) uniform CameraData
{
    mat4 proj;
    mat4 view;
    vec3 view_position;
    float pad1; // padding for std140
};

layout(std140) uniform DirectionalLightBlock
{
    vec3 direction; float pad1;
    vec3 ambient;   float pad2;
    vec3 diffuse;   float pad3;
    vec3 specular;  float pad4;
    mat4 light_space_matrix;
} dirLight;

layout(std140) uniform FogDataBlock
{
    vec3 fogColor; float pad1;   // vec3 padded to 16 bytes
    float fogStart;
    float fogEnd;
    float fogDensity;
    float pad2;                  // padding to make total multiple of 16 bytes
} fogData;

//uniform sampler2D diffuseTexture;

void main()
{
    gPosition = vec4(FragPos, 1.0);
    gNormal   = vec4(normalize(Normal), 1.0);
    //gAlbedo   = texture(diffuseTexture, TexCoords);
}


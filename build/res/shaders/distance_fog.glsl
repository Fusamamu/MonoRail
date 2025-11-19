#shader vertex
#version 330 core
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texCoord;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoord;

layout (std140) uniform Matrices
{
    mat4 proj;
    mat4 view;
};
uniform mat4 model;

void main()
{
    gl_Position = proj * view * model * vec4(aPos, 1.0);
    FragPos = vec3(model * vec4(position, 1.0)); // World-space position
    Normal  = mat3(transpose(inverse(model))) * aNormal;
    TexCoord = texCoord;
}

#shader fragment
#version 330 core
layout(location = 0) out vec4 FragColor;

in vec3 FragPos;    // position of the fragment in world space
in vec3 Normal;
in vec2 TexCoord;

uniform sampler2D diffuseTex;

//uniform vec3      cameraPos;
//uniform vec3      fogColor;
//uniform float     fogStart; // where fog begins
//uniform float     fogEnd;   // where fog is full

layout(std140) uniform FogSettings
{
    vec3  cameraPos;   // camera position in world space
    vec3  fogColor;    // fog color
    float fogStart;    // where fog begins
    float fogEnd;      // where fog is full
};

void main()
{
    vec4 baseColor = texture(diffuseTex, TexCoord);

    // Distance from camera
    float dist = length(FragPos - cameraPos);

    // Fog factor: 0 = no fog, 1 = full fog
    float fogFactor = clamp((fogEnd - dist) / (fogEnd - fogStart), 0.0, 1.0);

    // Mix object color with fog
    vec3 finalColor = mix(fogColor, baseColor.rgb, fogFactor);

    FragColor = vec4(finalColor, baseColor.a);
}


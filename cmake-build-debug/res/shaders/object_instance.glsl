#shader vertex
#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texCoord;

// Per-instance model matrix
// consumes locations 3,4,5,6
layout(location = 3) in mat4 instanceModel;
layout(location = 7) in vec4 instanceColor;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoord;
out vec4 Color;

layout(std140) uniform CameraData
{
    mat4 proj;
    mat4 view;
    vec3 view_position;
    float pad1; // padding for std140
};

uniform float u_time = 1.0;

void main()
{
    vec4 worldPos = instanceModel * vec4(position, 1.0);// Apply per-instance transform

    FragPos  = vec3(worldPos);
    Normal   = mat3(transpose(inverse(instanceModel))) * normal;
    TexCoord = texCoord;
    Color    = instanceColor;

    gl_Position = proj * view * worldPos;
}

#shader fragment
#version 330 core
layout(location = 0) out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;
in vec4 Color;

layout(std140) uniform CameraData
{
    mat4 proj;
    mat4 view;
    vec3 view_position;
    float pad1; // padding for std140
};

uniform vec3  u_fresnelColor = vec3(0.0, 1.0, 1.0);
uniform float u_fresnelPower = 10.0;

void main()
{
    // normalize
    vec3 N = normalize(Normal);
    vec3 V = normalize(view_position - FragPos);

    // Fresnel term
    float fresnel = pow(1.0 - max(dot(N, V), 0.0), u_fresnelPower);

    // Mix base color and fresnel color
    vec3 color = mix(Color.rgb, u_fresnelColor, fresnel);

    FragColor = vec4(color, Color.a);
}

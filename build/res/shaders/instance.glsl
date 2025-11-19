#shader vertex
#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texCoord;

// Per-instance model matrix
layout(location = 3) in mat4 instanceModel; // consumes locations 3,4,5,6

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoord;

layout(std140) uniform CameraData
{
    mat4 proj;
    mat4 view;
    vec3 view_position;
    float pad1; // padding for std140
};

uniform float u_time = 1.0;
uniform float u_wind_strength = 0.1;
uniform float u_blade_height = 1.0;

void main()
{
    // Compute wind offset based on vertex height
    float heightFactor = position.y / u_blade_height; // 0 at bottom, 1 at tip
    float sway = sin(u_time * 2.0 + instanceModel[3][0] * 10.0) * u_wind_strength * heightFactor;

    // Apply wind along X axis
    vec3 offsetPos = position + vec3(sway, 0.0, 0.0);

    // Apply per-instance transform
    vec4 worldPos = instanceModel * vec4(offsetPos, 1.0);

    FragPos  = vec3(worldPos);
    Normal   = mat3(transpose(inverse(instanceModel))) * normal;
    TexCoord = texCoord;

    gl_Position = proj * view * worldPos;
}

#shader fragment
#version 330 core
layout(location = 0) out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;

uniform float bladeHeight = 1.0; // maximum height of the grass blade

void main()
{
    // Normalize y coordinate from 0 (bottom) to 1 (top)
    float t = clamp(FragPos.y / bladeHeight, 0.0, 1.0);

    // Bottom color (darker green)
    vec3 bottomColor = vec3(0.1, 0.5, 0.1);
    // Top color (lighter green)
    vec3 topColor = vec3(0.3, 1.0, 0.3);

    // Linear interpolation
    vec3 color = mix(bottomColor, topColor, t);

    FragColor = vec4(color, 1.0);
}

#shader vertex
#version 330 core
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;  // ignored for UI, but kept for compatibility
layout(location = 2) in vec2 texCoord;

out vec2 vTexCoord;
out vec4 vColor;

uniform mat4 proj;
uniform mat4 model;
uniform vec4 u_color;

void main()
{
    vTexCoord   = texCoord;
    vColor      = u_color;
    gl_Position = proj * model * vec4(position, 1.0);
}

#shader fragment
#version 330 core
in vec2 vTexCoord;
in vec4 vColor;

out vec4 FragColor;

uniform sampler2D u_texture;
uniform bool uUseTexture;

void main()
{
    // Rectangle properties
    vec2 rectCenter   = vec2(0.5, 0.5);
    vec2 rectHalfSize = vec2(0.48, 0.48);
    float radius      = 0.02;

    vec3 rectColor = vec3(0.149, 0.125, 0.114);

    // Rounded rectangle SDF
    vec2 d = abs(vTexCoord - rectCenter) - rectHalfSize;
    float sdf = length(max(d, 0.0)) + min(max(d.x,d.y), 0.0) - radius;

    // Smooth edges
    float edge = 0.003;
    float alpha = 1.0 - smoothstep(0.0, edge, sdf);

    FragColor = vec4(rectColor, alpha);
}


#shader vertex
#version 330 core
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;  // ignored for UI, but kept for compatibility
layout(location = 2) in vec2 texCoord;

out vec2 vTexCoord;
out vec4 vColor;

uniform mat4 proj; // orthographic projection
uniform mat4 model;      // model matrix for quad transform
uniform vec4 u_color;      // color for the quad

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

uniform sampler2D uTexture;
uniform bool uUseTexture;
//uniform float u_radius;   // corner radius in normalized quad coordinates [0.0-1.0]

void main()
{
    float u_radius = 0.5;

    // Center of the quad in UV space
    vec2 center = vec2(0.5, 0.5);

    // Distance from current fragment to center
    float dist = length(vTexCoord - center);

    // Smooth edge using smoothstep for anti-aliasing
    float alpha = smoothstep(u_radius, u_radius - 0.01, dist);

        alpha = dist;
    vec4 color = vec4(1.0);


    color.a *= alpha;  // apply circular alpha

    FragColor = color;
}


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
uniform int  u_levels = 9;
uniform vec3 u_base_color   = vec3(0.0, 1.0, 0.0);
uniform vec3 u_effect_color = vec3(0.2, 0.2, 0.2);

void main()
{
    float noiseValue = texture(u_texture, vTexCoord).r;

    float level = floor(noiseValue * float(u_levels));

    float mask = level / float(u_levels - 1); // back to 0-1 range

    vec3 color = mix(u_base_color, u_effect_color, mask);

    FragColor = vec4(color, 1.0);
}


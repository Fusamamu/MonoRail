#shader vertex
#version 330 core
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texCoord;

out vec2 TexCoord;

void main()
{
    gl_Position = vec4(position.x, position.y, 0.0, 1.0);
    TexCoord = texCoord;
}

#shader fragment
#version 330 core
layout(location = 0) out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D u_depth_texture;

// near/far plane for linearizing depth
uniform float near_plane;
uniform float far_plane;

uniform float u_focus_dist;
uniform float u_focus_range;
//uniform float u_max_blur;

float LinearizeDepth(float depth)
{
    float z = depth * 2.0 - 1.0; // back to NDC
    return (2.0 * near_plane * far_plane) /
    (far_plane + near_plane - z * (far_plane - near_plane));
}

void main()
{
    float depthValue = texture(u_depth_texture, TexCoord).r;

    // Linearize for debugging:
    float linearDepth = LinearizeDepth(depthValue) / far_plane;

    float coc = (linearDepth - u_focus_dist) / u_focus_range;

    vec3 color;
    if (coc < 0.0)
    {
        // NEAR blur → Red
        float strength = abs(coc);
        color = mix(vec3(1.0), vec3(1.0, 1.0 - strength, 1.0 - strength), strength);
    }
    else
    {
        // FAR blur → Green
        float strength = coc;
        color = mix(vec3(1.0), vec3(1.0 - strength, 1.0, 1.0 - strength), strength);
    }

    FragColor = vec4(vec3(color), 1.0);
}

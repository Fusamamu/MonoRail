#shader vertex
#version 330 core
layout(location = 0) in vec2 position;
layout(location = 1) in vec2 texCoord;

out vec2 vTexCoord;
out vec4 vColor;

uniform mat4 proj;
uniform mat4 model;
uniform vec4 u_color;

void main()
{
    vTexCoord   = texCoord;
    vColor      = u_color;
    gl_Position = proj * model * vec4(position, 0.0, 1.0);
}

#shader fragment
#version 330 core
in vec2 vTexCoord;
in vec4 vColor;

out vec4 FragColor;

uniform sampler2D u_texture; // your MSDF atlas
uniform bool uUseTexture;    // optional flag if you mix text and colored quads

// Optional tweakable edge threshold — smaller = sharper edges, larger = smoother
uniform float uSmoothness = 0.85;

float median(float r, float g, float b)
{
    return max(min(r, g), min(max(r, g), b));
}

void main()
{
    vec3 sdf = texture(u_texture, vTexCoord).rgb;

    // Compute signed distance from multi-channel
    float sd = median(sdf.r, sdf.g, sdf.b);

    // Center around 0.5
    float screenPxDistance = uSmoothness * fwidth(sd);
    float alpha = smoothstep(0.5 - screenPxDistance, 0.5 + screenPxDistance, sd);

    // Output final color with alpha based on MSDF distance
    FragColor = vec4(vColor.rgb, vColor.a * alpha);
}

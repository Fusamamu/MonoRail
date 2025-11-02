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

void main()
{
    vec4 color = texture(u_texture, vTexCoord);
    FragColor = color;

//    float noiseValue = texture(u_texture, vTexCoord).r; // take red channel
//    FragColor = vec4(noiseValue, noiseValue, noiseValue, 1.0); // grayscale
}


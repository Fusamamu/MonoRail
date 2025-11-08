#shader vertex
#version 330 core
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texCoord;

out vec3 world_pos;
out vec2 TexCoord;

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
    vec4 _world = model * vec4(position, 1.0);
    world_pos = _world.xyz;
    TexCoord = texCoord;
    gl_Position = proj * view * _world;
}

#shader fragment
#version 330 core
layout(location = 0) out vec4 FragColor;

in vec3 world_pos;
in vec2 TexCoord;

uniform float u_scale;
uniform sampler2D u_texture;
uniform int  u_levels = 9;

void main()
{
    vec2 _uv = world_pos.xz * u_scale;
    _uv = fract(_uv);

    float noiseValue = texture(u_texture, _uv).r;
    float level = floor(noiseValue * float(u_levels));

float mask = level / float(u_levels - 1); // back to 0-1 range

    FragColor = vec4(vec3(mask), 1.0);
}


#shader vertex
#version 330 core
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 color;

layout(std140) uniform CameraData
{
    mat4 proj;
    mat4 view;
    vec3 view_position;
    float pad1; // padding for std140
};

void main()
{
    gl_Position = proj * view * vec4(position, 1.0);
}

#shader fragment
#version 330 core
layout(location = 0) out vec4 FragColor;

void main()
{
    FragColor = vec4(vec3(1.0, 0.0, 0.0), 1.0);
}


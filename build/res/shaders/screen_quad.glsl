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

uniform sampler2D u_screen_texture;

void main()
{
    FragColor = texture(u_screen_texture, TexCoord);
}

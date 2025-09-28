#shader vertex
#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texCoord;

out vec2 TexCoord;
out vec3 FragPos;
out vec3 Normal;

uniform mat4 proj;
uniform mat4 view;
uniform mat4 model;

void main()
{
    vec4 worldPos = model * vec4(position, 1.0);

    FragPos = worldPos.xyz;
    Normal  = mat3(transpose(inverse(model))) * normal; // transform normal to world space

    TexCoord = texCoord;
    gl_Position = proj * view * worldPos;
}

#shader fragment
#version 330 core

in vec2 TexCoord;
in vec3 FragPos;
in vec3 Normal;

out vec4 color;

uniform vec3 LightDir    = normalize(vec3(-0.5, -1.0, -0.3));
uniform vec3 LightColor  = vec3(1.0, 1.0, 1.0);
uniform vec3 ObjectColor = vec3(1.0, 0.5, 0.2);

void main()
{
    // Normalize normal
    vec3 N = normalize(Normal);
    vec3 L = normalize(-LightDir); // Light direction toward surface

    // Compute diffuse intensity
    float diff = max(dot(N, L), 0.0);

    // Quantize the intensity for toon effect
    if (diff > 0.95)       diff = 1.0;
    else if (diff > 0.5)   diff = 0.7;
    else if (diff > 0.25)  diff = 0.4;
    else                   diff = 0.1;

    vec3 result = diff * LightColor * ObjectColor;
    color = vec4(result, 1.0);
}



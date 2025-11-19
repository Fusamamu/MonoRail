#shader vertex
#version 330 core
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texCoord;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoord;

layout(std140) uniform CameraData
{
    mat4 proj;
    mat4 view;
    vec3 view_position;
    float pad1; // padding for std140
};

uniform mat4 model;

void main()
{
    FragPos  = vec3(model * vec4(position, 1.0));
    Normal   = mat3(transpose(inverse(model))) * normal;
    TexCoord = texCoord;
    gl_Position = proj * view * vec4(FragPos, 1.0);
}

#shader fragment
#version 330 core
layout(location = 0) out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;

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
} dirLight;

layout(std140) uniform FogDataBlock
{
    vec3 fogColor; float pad1;   // vec3 padded to 16 bytes
    float fogStart;
    float fogEnd;
    float fogDensity;
    float pad2;                  // padding to make total multiple of 16 bytes
} fogData;

uniform vec2 u_screen_size;

uniform float u_shininess;


uniform sampler2D u_color_texture;
uniform sampler2D u_depth_texture;

uniform float u_near_plane;
uniform float u_far_plane;

uniform float u_threshold;

// Convert depth buffer value (0..1) back to view-space distance
float LinearizeDepth(float depth, float nearPlane, float farPlane)
{
    float z = depth * 2.0 - 1.0; // [0..1] -> NDC [-1..1]
    return (2.0 * nearPlane * farPlane) /
    (farPlane + nearPlane - z * (farPlane - nearPlane));
}

vec3 CalculateDirLight(vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-dirLight.direction);

    float diff = max(dot(normal, lightDir), 0.0);

    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), u_shininess);

    vec3 ambient  = dirLight.ambient;
    vec3 diffuse  = dirLight.diffuse  * diff;
    vec3 specular = dirLight.specular * spec;

    return (ambient + diffuse + specular);
}

void main()
{
    vec2 screenUV = gl_FragCoord.xy / u_screen_size;
    float z_ndc = texture(u_depth_texture, screenUV).r * 2.0 - 1.0;
    float x_ndc = screenUV.x * 2.0 - 1.0;
    float y_ndc = screenUV.y * 2.0 - 1.0;

    vec4 ndcPos   = vec4(x_ndc, y_ndc, z_ndc, 1.0);
    vec4 viewPos  = inverse(proj) * ndcPos;
    viewPos /= viewPos.w;
    vec3 worldPos = (inverse(view) * viewPos).xyz;

    // Compute fog factor based on Y (world-space height)
    float fogFactor = clamp((worldPos.y - fogData.fogStart) / (fogData.fogEnd - fogData.fogStart), 0.0, 1.0);

    vec3 sceneColor = texture(u_color_texture, screenUV).rgb;

    // Blend fog with fragment color
    vec3 finalColor = mix(fogData.fogColor, sceneColor, fogFactor);

    FragColor = vec4(finalColor, 1.0);
}


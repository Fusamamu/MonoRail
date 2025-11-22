#shader vertex
#version 330 core
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texCoord;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoord;
out vec4 FragPosLightSpace;

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
    vec4 worldPos = model * vec4(position, 1.0);
    FragPos  = worldPos.xyz;
    Normal   = mat3(transpose(inverse(model))) * normal;
    TexCoord = texCoord;

    FragPosLightSpace = dirLight.light_space_matrix * worldPos;

    gl_Position = proj * view * vec4(FragPos, 1.0);
}

#shader fragment
#version 330 core
layout(location = 0) out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;
in vec4 FragPosLightSpace;

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

layout(std140) uniform FogDataBlock
{
    vec3 fogColor; float pad1;   // vec3 padded to 16 bytes
    float fogStart;
    float fogEnd;
    float fogDensity;
    float pad2;                  // padding to make total multiple of 16 bytes
} fogData;

uniform vec3      u_color;
uniform float     u_shininess;
uniform int       u_levels;

uniform sampler2D u_shadow_map;
uniform sampler2D u_texture;
uniform sampler3D u_voxel_ao;

uniform vec3      u_voxelMin;    // world-space minimum corner of voxel grid
uniform vec3      u_voxelMax;    // world-space maximum corner of voxel grid
uniform int       u_voxelResolution; // size of 3D AO grid


float calculate_shadow(vec4 fragPosLightSpace, vec3 normal, vec3 lightDir)
{
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w; // Perform perspective divide

    projCoords = projCoords * 0.5 + 0.5;    // Transform to [0,1] range

    // If outside light’s frustum
    if (projCoords.z > 1.0)
        return 0.0;

    // Depth in shadow map
    float closestDepth = texture(u_shadow_map, projCoords.xy).r;
    float currentDepth = projCoords.z;

    float bias   = max(0.0005 * (1.0 - dot(normal, -lightDir)), 0.0005);  // Bias to avoid acne (depends on surface angle)
    float shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;  // Shadow = 1.0 if in shadow, 0.0 if lit

    return shadow;
}

vec3 calculate_dir_light(vec3 normal, vec3 viewDir, float shadow)
{
    vec3 lightDir = normalize(-dirLight.direction);

    // Diffuse
    float diff = max(dot(normal, lightDir), 0.0);

    // Specular
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), u_shininess);

    // Combine
    vec3 ambient  = dirLight.ambient;
    vec3 diffuse  = dirLight.diffuse  * diff * (1.0 - shadow);
    vec3 specular = dirLight.specular * spec * (1.0 - shadow);

    return (ambient + diffuse + specular);
}

void main()
{
    vec3 _norm      = normalize(Normal);
    vec3 _view_dir  = normalize(view_position - FragPos);
    vec3 _light_dir = normalize(-dirLight.direction);

    float shadow   = calculate_shadow(FragPosLightSpace, _norm, _light_dir);
    vec3 _result   = calculate_dir_light(_norm, _view_dir, shadow);

    /* --- Compute linear fog factor ---*/
    float distance  = length(view_position - FragPos);
    float fogFactor = clamp((fogData.fogEnd - distance) / (fogData.fogEnd - fogData.fogStart), 0.0, 1.0);

    vec3 color = mix(fogData.fogColor, _result, fogFactor);
    color = _result * u_color;

    vec2 _uv = FragPos.xz * 0.1f;
    _uv = fract(_uv);

    float noiseValue = texture(u_texture, _uv).r;
    float level      = floor(noiseValue * float(u_levels));

    float mask = level / float(u_levels - 1); // back to 0-1 range

    float _color = mask * (1.0 - shadow);

    //FragColor = vec4(vec3(_color), 1.0);


    //vec3 voxelUV = (FragPos - u_voxelMin) / (u_voxelMax - u_voxelMin);


//    vec3 voxelUV = vec3
//    (
//        (FragPos.z - u_voxelMin.z) / (u_voxelMax.z - u_voxelMin.z),
//        (FragPos.y - u_voxelMin.y) / (u_voxelMax.y - u_voxelMin.y),
//        (FragPos.x - u_voxelMin.x) / (u_voxelMax.x - u_voxelMin.x)
//    );


vec3 voxelUV = vec3
(
(FragPos.z - u_voxelMin.z) / (u_voxelMax.z - u_voxelMin.z),   // X stays the same?
(FragPos.y - u_voxelMin.y) / (u_voxelMax.y - u_voxelMin.y),   // Y stays the same
1.0 - ( (FragPos.x - u_voxelMin.x) / (u_voxelMax.x - u_voxelMin.x) ) // <-- flipped Z
);

float angle = radians(270);    // you can pass angle in degrees
float c = cos(angle);
float s = sin(angle);

float x = voxelUV.x;
float z = voxelUV.z;

voxelUV.x =  c * x + s * z;
voxelUV.y =  voxelUV.y;   // unchanged
voxelUV.z = -s * x + c * z;








voxelUV = clamp(voxelUV, 0.0, 1.0); // ensure inside grid


    float ao = texture(u_voxel_ao, voxelUV).r; // AO stored in red channel

        _color *= ao;


    FragColor = vec4(vec3(ao), 1.0);

    FragColor = vec4(vec3(_color), 1.0);
}


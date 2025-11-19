#shader vertex
#version 330 core
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texCoord;

// Pass to fragment
out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoord;

out float vGradient;    // 0 base → 1 tip
out vec3 vWorldPos;

// Camera uniform (already defined)
layout(std140) uniform CameraData
{
    mat4 proj;
    mat4 view;
    vec3 view_position;
    float pad1;
};

uniform mat4 model;

// Layer / instance uniforms
uniform float layerSpacing = 1.0;   // height per layer
uniform int numLayers = 8;          // total layers

void main()
{
    int layer = gl_InstanceID;                      // current instance layer

    float layerT = float(layer) / float(numLayers-1); // 0..1
    vGradient = layerT;

    vec3 offset = vec3(0.0, layerT * layerSpacing, 0.0);

    vec4 worldPos = model * vec4(position + offset, 1.0);
    vWorldPos = worldPos.xyz;

    FragPos = worldPos.xyz;
    Normal = mat3(transpose(inverse(model))) * normal;
    TexCoord = texCoord;

    gl_Position = proj * view * worldPos;
}


#shader fragment
#version 330 core
layout(location = 0) out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;
in float vGradient;
in vec3 vWorldPos;

//// Grass textures
uniform sampler2D grassPattern;  // scrolling fur/grass strokes
//uniform sampler2D grassCutout;   // static holes
//uniform sampler2D trailMask;     // orthographic top-down player trail
//uniform sampler2D depthMap;      // optional depth from ortho camera
//
//// Ortho camera matrix to project world → trail UV
//uniform mat4 orthoVP;
//
//// Animation / movement
uniform float u_time;
uniform vec2 patternScroll = vec2(0.2, 0.0);
//

uniform float alphaClip = 0.9;

void main()
{
//    // --- Scrolling pattern ---
    vec2 patternUV = TexCoord + patternScroll * u_time;
    float _pattern = texture(grassPattern, patternUV).r;


    //float _pattern = texture(grassPattern, TexCoord).r;


//    // --- Static cutout ---
//    float cutout = texture(grassCutout, TexCoord).r;
//
//    // --- Combine pattern, cutout, gradient ---
//    float alpha = mix(1.0, pattern * cutout, vGradient);
    float _alpha = mix(1.0, _pattern, vGradient);
//
//    // --- Trail mask sampling ---
//    vec4 orthoPos = orthoVP * vec4(vWorldPos, 1.0);
//    vec2 trailUV = orthoPos.xy / orthoPos.w * 0.5 + 0.5;
//    float trail = texture(trailMask, trailUV).r;
//
//    // Optional: use depth map to reduce artifacts
//    // float depthSample = texture(depthMap, trailUV).r;
//    // alpha *= step(FragPos.y, depthSample);
//
//    // Reduce alpha where trail is present
//    alpha = mix(alpha, alpha * 0.1, trail);
//
    if(_alpha < alphaClip)
        discard;
    // Simple grass color
    vec3 baseColor = vec3(0.1, 0.7, 0.1);
    FragColor = vec4(baseColor * (0.6 + _pattern * 0.4), _alpha);
}


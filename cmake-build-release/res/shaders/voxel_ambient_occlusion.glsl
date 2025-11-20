#shader vertex
#version 330 core
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texCoord;

out vec3 FragPos;
out vec2 TexCoord;

uniform mat4 proj;
uniform mat4 model;

void main()
{
    TexCoord    = texCoord;
    gl_Position = proj * model * vec4(position, 1.0);
}

#shader fragment
#version 330 core
layout(location = 0) out vec4 FragColor;

in vec3 FragPos;
in vec2 TexCoord;


uniform float     u_slice;
uniform sampler3D u_texture;//voxel_texture!!

uniform int       u_voxel_resolution;
uniform int       u_num_rays;
uniform int       u_max_steps;
uniform float     u_step_length;

float hash(float _n)
{
    return fract(sin(_n) * 43748.5453132); 
}

// generate a random direction roughly upward hemisphere
vec3 sampleDirection(float _seed) 
{
    float phi = 6.2831853 * hash(_seed);
    float z   = hash(_seed * 3.13) * 2.0 - 1.0;
    float r   = sqrt(max(0.0, 1.0 - z*z));
    return vec3(r * cos(phi), r * sin(phi), z);
}

void main()
{
//    // 0..1 texture coordinate for current voxel in XY and slice in Z
//    vec3 texCoord = vec3(TexCoord.x, float(u_slice) / float(u_voxel_resolution), TexCoord.y);
//
//    // Skip if this voxel is empty
//    float occ = texture(u_voxel_texture, texCoord).r;
//    if (occ < 0.5)
//    {
//        FragColor = vec4(1.0, 0.0, 0.0, 1.0); // fully open space (no AO)
//        return;
//    }
//
//    // Voxel position in voxel units
//    vec3 pos = texCoord * float(u_voxel_resolution);
//
//    float visible = 0.0;
//    for (int r = 0; r < u_max_steps; ++r)
//    {
//        // Randomized direction
//        vec3 dir = normalize(sampleDirection(float(r) + pos.x + pos.y + pos.z));
//
//        float vis = 1.0;
//        vec3 p = pos;
//
//        for (int s = 0; s < u_max_steps; ++s)
//        {
//            p += dir * u_step_length;
//            vec3 tc = p / float(u_voxel_resolution);
//
//            // Stop if we go outside
//            if (any(lessThan(tc, vec3(0.0))) || any(greaterThan(tc, vec3(1.0))))
//                break;
//
//            // Sample voxel density
//            float d = texture(u_voxel_texture, tc).r;
//            if (d > 0.5)
//            {
//                vis = 0.0; break;
//            } // hit solid voxel
//        }
//
//        visible += vis;
//    }
//
//    float ao = visible / float(u_num_rays);
//    FragColor = vec4(ao, 0.0, 0.0, 1.0); // AO stored in red

    FragColor = vec4(1.0, 0.0, 0.0, 1.0); // AO stored in red
}


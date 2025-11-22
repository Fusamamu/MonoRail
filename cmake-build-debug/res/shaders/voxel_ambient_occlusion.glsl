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
    gl_Position = vec4(position.x, position.y, 0.0, 1.0);
    TexCoord    = texCoord;
}

#shader fragment
#version 330 core
layout(location = 0) out vec4 FragColor;

in vec3 FragPos;
in vec2 TexCoord;

uniform float     u_slice;
uniform float     u_slice_norm;
uniform sampler3D u_texture;//voxel_texture!!

uniform int       u_voxel_resolution;
uniform int       u_num_rays;
uniform int       u_max_steps;
uniform float     u_step_length;

float hash(float _n)
{
    return fract(sin(_n) * 43748.5453132);
}

vec3 sampleDirection(float _seed)
{
    float phi = 6.2831853 * hash(_seed);
    float z   = hash(_seed * 3.13) * 2.0 - 1.0;
    float r   = sqrt(max(0.0, 1.0 - z*z));
    return vec3(r * cos(phi), r * sin(phi), z);
}

void main()
{
    vec3 texCoord = vec3(TexCoord.x, TexCoord.y, float(u_slice) / float(u_voxel_resolution));

    float solid = texture(u_texture, texCoord).r;

    if (solid > 0.5) {
        FragColor = vec4(0.0, 0.0, 0.0, 1.0);
        return;
    }

    // Voxel coordinate in voxel grid space
    vec3 pos = texCoord * float(u_voxel_resolution);

    float visible = 0.0;

    for (int r = 0; r < u_num_rays; ++r) {

        vec3 dir = normalize(sampleDirection(float(r) + pos.x + pos.y + pos.z));

        // track if this ray is unobstructed
        float ray_vis = 1.0;

        vec3 p = pos;

        for (int s = 0; s < u_max_steps; ++s)
        {
            p += dir * u_step_length;
            vec3 tc = p / float(u_voxel_resolution);

            // outside grid → open space
            if (any(lessThan(tc, vec3(0.0))) || any(greaterThan(tc, vec3(1.0))))
            {
                break;
            }

            // sample solid voxels
            float d = texture(u_texture, tc).r;

            if (d > 0.5)
            {
                ray_vis = 0.0;
                break;
            }
        }

        visible += ray_vis;
    }

    float ao = visible / float(u_num_rays);

    FragColor = vec4(ao, 0.0, 0.0, 1.0);
}

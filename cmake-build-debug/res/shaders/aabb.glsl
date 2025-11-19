#shader vertex
#version 330 core
layout(location = 0) in vec3 aMin;
layout(location = 1) in vec3 aMax;

out VS_OUT {
    vec3 min;
    vec3 max;
} vs_out;

void main()
{
    vs_out.min = aMin;
    vs_out.max = aMax;

    // Trick: Emit a dummy point
    gl_Position = vec4(0.0); // Doesn't matter
}

#shader geometry
#version 330 core
layout(points) in;
layout(line_strip, max_vertices = 24) out;

layout(std140) uniform CameraData
{
    mat4 proj;
    mat4 view;
    vec3 view_position;
    float pad1; // padding for std140
};

uniform mat4 model;

in VS_OUT {
    vec3 min;
    vec3 max;
} gs_in[];

void emitLine(vec3 a, vec3 b, mat4 viewProj)
{
    gl_Position = viewProj * vec4(a, 1.0);
    EmitVertex();
    gl_Position = viewProj * vec4(b, 1.0);
    EmitVertex();
    EndPrimitive();
}

void main()
{
    vec3 min = gs_in[0].min;
    vec3 max = gs_in[0].max;

    vec3 corners[8];
    corners[0] = vec3(min.x, min.y, min.z);
    corners[1] = vec3(max.x, min.y, min.z);
    corners[2] = vec3(max.x, max.y, min.z);
    corners[3] = vec3(min.x, max.y, min.z);
    corners[4] = vec3(min.x, min.y, max.z);
    corners[5] = vec3(max.x, min.y, max.z);
    corners[6] = vec3(max.x, max.y, max.z);
    corners[7] = vec3(min.x, max.y, max.z);

    // Precompute combined matrix
    mat4 viewProj = proj * view;

    for (int i = 0; i < 8; ++i)
        corners[i] = vec3(model * vec4(corners[i], 1.0));

    // Edges: 12 lines
    int edges[24] = int[]
    (
        0, 1, 1, 2, 2, 3, 3, 0, // Bottom face
        4, 5, 5, 6, 6, 7, 7, 4, // Top face
        0, 4, 1, 5, 2, 6, 3, 7  // Vertical edges
    );

    for (int i = 0; i < 24; i += 2)
    {
        emitLine(corners[edges[i]], corners[edges[i + 1]], viewProj);
    }
}

#shader fragment
#version 330 core
out vec4 FragColor;

//uniform vec3 uColor;

void main()
{
        //uColor = vec3(1.0);
    FragColor = vec4(vec3(1.0), 1.0);
}


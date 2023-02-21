#version 430
#define MAX_INDEX 2000000

layout(local_size_x = 1, local_size_y = 1) in;
layout(rgba32f, binding = 0) uniform image2D img_output;
layout(std430, binding = 4) buffer trailLayout
{
    vec4 trail[];
};

void main() {
    uvec2 index = gl_GlobalInvocationID.xy;
    uint trail_index = index.x + index.y * 2000;        
        
    vec4 blur = vec4(0, 0, 0, 0);
    blur += trail[trail_index];
    blur += trail[(trail_index - 1) % MAX_INDEX];
    blur += trail[(trail_index - 2000) % MAX_INDEX];
    blur += trail[(trail_index - 2001) % MAX_INDEX];
    blur += trail[(trail_index - 1999) % MAX_INDEX];
    blur += trail[(trail_index + 1) % MAX_INDEX];
    blur += trail[(trail_index + 2000) % MAX_INDEX];
    blur += trail[(trail_index + 2001) % MAX_INDEX];
    blur += trail[(trail_index + 1999) % MAX_INDEX];

    blur /= 9.0f;

    trail[trail_index] = blur * 0.95f;
    imageStore(img_output, ivec2(index), trail[trail_index]);
}
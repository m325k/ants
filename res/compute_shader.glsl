#version 430

#define PI 3.14159265359
#define SEARCH_SPACE 0.025
#define NUM_OF_SAMPLES 100
#define speed sqrt(2 * 0.001 * 0.001) 

uniform float oracle;

layout(local_size_x = 1, local_size_y = 1) in;
layout(rgba32f, binding = 0) uniform image2D img_output;
layout(std430, binding = 2) buffer postionLayout
{
    vec2 positions[];
};

layout(std430, binding = 3) buffer velocityLayout
{
    vec2 velocities[];
};

layout(std430, binding = 4) buffer trailLayout
{
    vec4 trail[];
};

float random (in float x) {
    return fract(sin(x)*1e4);
}

float rand(float from, float to, int i) {
    float r = random(oracle * random(i + 1));
    return (to - from) * r + from;
}

mat2 rotate(float theta){
    return mat2(cos(theta), -sin(theta),
                sin(theta), cos(theta));
}
    
void main() {
    uint NUM_OF_AGENTS = gl_NumWorkGroups.x * gl_NumWorkGroups.y;

    // base pixel colour for image
    vec4 pixel_color = vec4(0.5, 0.0, 0.5, 1.0);

    uint index = gl_GlobalInvocationID.x;
    if(index > NUM_OF_AGENTS || index < 0) return;


    vec2 pixel_coords = positions[index];

    vec2 vel = velocities[index];
    if(abs(vel.x) < 0.0000001 && abs(vel.y) < 0.0000001) {
        pixel_color.z = 30;
        pixel_color.x = 0;
    }

    vec2 yAxis = normalize(vel);
    vec2 xAxis = yAxis * rotate(PI / 2);      

    vec4 left    = vec4(0, 0, 0, 0);
    vec4 right   = vec4(0, 0, 0, 0);
    vec4 forward = vec4(0, 0, 0, 0);
    int left_count = 0;
    int right_count = 0;
    int forward_count = 0; 
    for(int i = 0;i < NUM_OF_SAMPLES;i++) {
        vec2 rand_vec = vec2(rand(-SEARCH_SPACE, SEARCH_SPACE, i * int(index)), rand(SEARCH_SPACE / 5, 2 * SEARCH_SPACE, int(index) * i * NUM_OF_SAMPLES));
        vec2 rand_rel_position =  rand_vec.x * xAxis +  rand_vec.y * yAxis;
        vec2 rand_abs_position = rand_rel_position + pixel_coords;
        uint trail_index = uint(rand_abs_position.x * 2000) + uint(rand_abs_position.y * 1000) * 2000;

        float theta = atan(rand_vec.y, rand_vec.x);
        if(theta > 7 * PI / 12) {
            right_count++;
            right += trail[trail_index];
            //trail[trail_index].y = 1.0f;
        } else if(theta < 5 * PI / 12) {
            left_count++;
            left += trail[trail_index];
            //trail[trail_index].x = 1.0f;
        } else {
            forward_count++;
            forward += trail[trail_index];
        }
    }
      
    float left_conc    = (left_count == 0)    ? -1 : left.z / float(left_count);
    float right_conc   = (right_count == 0)   ? -1 : right.z / float(right_count);
    float forward_conc = (forward_count == 0) ? 0 : forward.z / float(forward_count); 


    float rot_main = PI / 128;
    float rot_side = PI / (12);

    if(forward_conc >= left_conc - 1.0f/NUM_OF_SAMPLES && forward_conc >= right_conc - 1.0f/NUM_OF_SAMPLES) {
        //do nothing
    } else if (forward_conc < left_conc && forward_conc < right_conc) {
        //Mutate
        velocities[index] *= rotate(rand(-rot_main, rot_main, int(index) * 325));
    } else if(left_conc > right_conc) {
        velocities[index] *= rotate(rot_side);
    } else if(right_conc > left_conc) {
        velocities[index] *= rotate(-rot_side);
    }

    //velocities[index] = normalize(velocities[index]) * speed;

    vel = velocities[index] * 1.0004;

    vec2 new_coords = pixel_coords + vel;

    if(new_coords.x > 1.0f) {
        new_coords.x = 0.0f;
    }

    if(new_coords.x < 0.0f) {
        new_coords.x = 1.0f;
    }

    if(new_coords.y > 1.0f) {
        new_coords.y = 0.0f;
    }

    if(new_coords.y < 0.0f) {
        new_coords.y = 1.0f;
    }

    positions[index] = new_coords;

    ivec2 texture_coords = ivec2(int(new_coords.x * 2000), int(new_coords.y * 1000));

    trail[texture_coords.x + texture_coords.y * 2000] += pixel_color;
}
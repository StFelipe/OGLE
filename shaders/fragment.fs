#version 330 core

precision highp float; 
uniform vec2 u_resolution;  // Width and height of the shader
uniform float u_time;  // Time elapsed

uniform vec3 cameraPos;
uniform vec3 cameraDir;
uniform vec3 cameraRight;
uniform vec3 cameraUp;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

// Constants
#define NUMBER_OF_STEPS 64
#define MINIMUM_HIT_DISTANCE 0.001
#define MAXIMUM_TRACE_DISTANCE 1000.0
 
 float distance_from_sphere(in vec3 pos, in vec3 spherePos, float r)
{
	return length(mod(pos, 2    ) - vec3(1, 1, 1)) - .5;
    // spherePos.xy = mod((spherePos.xy),1.0)-vec2(0.5); // instance on xy-plane
    // return length(spherePos)-0.3;             // sphere DE
}

float map_the_world(in vec3 p)
{
    vec4 pos = projection * view * model * vec4(0, 0, 0, 1);
    float sphere_0 = distance_from_sphere(p, vec3(pos.xyz), 1);
    
    return sphere_0;
}

vec3 calculate_normal(in vec3 p)
{
    const vec3 small_step = vec3(0.001, 0.0, 0.0);

    float gradient_x = map_the_world(p + small_step.xyy) - map_the_world(p - small_step.xyy);
    float gradient_y = map_the_world(p + small_step.yxy) - map_the_world(p - small_step.yxy);
    float gradient_z = map_the_world(p + small_step.yyx) - map_the_world(p - small_step.yyx);

    vec3 normal = vec3(gradient_x, gradient_y, gradient_z);

    return normalize(normal);
}



vec3 ray_march(in vec3 ro, in vec3 rd)
{
    float total_distance_traveled = 0.0;

    for (int i = 0; i < NUMBER_OF_STEPS; ++i)
    {
        // Calculate our current position along the ray
        vec3 current_position = ro + total_distance_traveled * rd;

        // We wrote this function earlier in the tutorial -
        // assume that the sphere is centered at the origin
        // and has unit radius
        vec4 pos = projection * view * model * vec4(0.0, 0.0, 0.0, 1);
        float distance_to_closest = map_the_world(current_position);

        if (distance_to_closest < MINIMUM_HIT_DISTANCE) // hit
        {
            vec3 normal = calculate_normal(current_position);

            // Remember, each component of the normal will be in 
            // the range -1..1, so for the purposes of visualizing
            // it as an RGB color, let's remap it to the range
            // 0..1
            //return vec3(1, 0, 0);
            return normal * 0.5 + 0.5;
        }

        if (total_distance_traveled > MAXIMUM_TRACE_DISTANCE) // miss
        {
            break;
        }

        // accumulate the distance traveled thus far
        total_distance_traveled += distance_to_closest;
    }

    // If we get here, we didn't hit anything so just
    // return a background color (black)
    return vec3(0.0);
}


void main()
{
    vec2 uv = (gl_FragCoord.xy - (0.5 * u_resolution.xy)) / u_resolution.x;

    vec3 ro = cameraPos;
    vec3 rd = cameraDir;
    rd += cameraRight * uv.x;
    rd += cameraUp * uv.y;

    vec3 color = ray_march(ro, rd);
    gl_FragColor = vec4(color,1.0);
}
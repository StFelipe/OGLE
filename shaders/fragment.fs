#version 330 core

precision highp float; 
uniform vec2 u_resolution;  // Width and height of the shader
uniform float u_time;  // Time elapsed

uniform vec3 cameraPos;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

// Constants
#define STEP_SIZE 0.1
#define NUMBER_OF_STEPS 32
#define MINIMUM_HIT_DISTANCE 0.001
#define MAXIMUM_TRACE_DISTANCE 1000.0
 
 float distance_from_sphere(in vec3 pos, in vec3 spherePos, float r)
{
	return length(spherePos - pos) - r;
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
        float distance_to_closest = distance_from_sphere(current_position, vec3(pos.xyz), 1.0);

        if (distance_to_closest < MINIMUM_HIT_DISTANCE) // hit
        {
            // We hit something! Return red for now
            return vec3(1.0, 0.0, 0.0);
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

    vec3 ro = vec3(0, 0, 0);
    vec3 rd = normalize(vec3(uv.x,uv.y,1));

    vec3 color = ray_march(ro, rd);
    gl_FragColor = vec4(color,1.0);
}
#version 330 core

precision highp float; 
uniform vec2 resolution;  // Width and height of the shader

uniform vec3 cameraPos;
uniform vec3 cameraDir;
uniform vec3 cameraRight;
uniform vec3 cameraUp;
uniform float moveSpeed;

uniform int numberOfSteps;

uniform float bailout;
uniform float power;
uniform int iterations;

// Constants
//#define NUMBER_OF_STEPS 250
//#define MINIMUM_HIT_DISTANCE 0.000000005
#define MAXIMUM_TRACE_DISTANCE 1000.0

// #define BAILOUT 5
// #define POWER 8
// #define ITERATIONS 16

#define MOVE_SPEED_HIT_DISTANCE_MUL 0.001
float minHitDistance;
 
 float distance_from_fractal(in vec3 pos)
{
	vec3 z = pos;
	float dr = 1.0;
	float r = 0.0;
	for (int i = 0; i < iterations ; i++) {
		r = length(z);
		if (r>bailout)
            break;
		
		// convert to polar coordinates
		float theta = acos(z.z/r);
		float phi = atan(z.y,z.x);
		dr =  pow( r, power-1.0)*power*dr + 1.0;
		
		// scale and rotate the point
		float zr = pow( r,power);
		theta = theta*power;
		phi = phi*power;
		
		// convert back to cartesian coordinates
		z = zr*vec3(sin(theta)*cos(phi), sin(phi)*sin(theta), cos(theta));
		z+=pos;
	}
	return 0.5*log(r)*r/dr;
}

// vec3 calculate_normal(in vec3 p)
// {
//     const vec3 small_step = vec3(0.001, 0.0, 0.0);

//     float gradient_x = distance_from_fractal(p + small_step.xyy) - distance_from_fractal(p - small_step.xyy);
//     float gradient_y = distance_from_fractal(p + small_step.yxy) - distance_from_fractal(p - small_step.yxy);
//     float gradient_z = distance_from_fractal(p + small_step.yyx) - distance_from_fractal(p - small_step.yyx);

//     vec3 normal = vec3(gradient_x, gradient_y, gradient_z);

//     return normalize(normal);
// }

vec3 ray_march(in vec3 ro, in vec3 rd)
{
    float total_distance_traveled = 0.0;

    for (int i = 0; i < numberOfSteps; ++i)
    {
        // Calculate our current position along the ray
        vec3 current_position = ro + total_distance_traveled * rd;

        // We wrote this function earlier in the tutorial -
        // assume that the sphere is centered at the origin
        // and has unit radius
        float distance_to_closest = distance_from_fractal(current_position);

        if (distance_to_closest < minHitDistance) // hit
        {
            // Remember, each component of the normal will be in 
            // the range -1..1, so for the purposes of visualizing
            // it as an RGB color, let's remap it to the range
            // 0..1
            //vec3 normal = calculate_normal(current_position);
            //return normal * 0.5 + 0.5;

            float c = (float(i) / numberOfSteps);
            return vec3(1 - c, abs(c - 0.5) * 2, 1 - (abs(c - 0.5) * 2));
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
    minHitDistance = moveSpeed * MOVE_SPEED_HIT_DISTANCE_MUL;

    vec2 uv = (gl_FragCoord.xy - (0.5 * resolution.xy)) / resolution.x;

    vec3 ro = cameraPos;
    vec3 rd = cameraDir;
    rd += cameraRight * uv.x;
    rd += cameraUp * uv.y;

    vec3 color = ray_march(ro, rd);
    gl_FragColor = vec4(color,1.0);
}
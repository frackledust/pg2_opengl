#version 460 core

// outputs
layout ( location = 0 ) out vec4 FragColor;

const float PI = 3.14159265359;
in vec3 world_normal;

uniform sampler2D texture_0;
void main( void )
{	
	float theta = acos(world_normal.z);
	float phi = atan(world_normal.y, world_normal.x);
	if (world_normal.y < 0)
		phi = phi + PI * 2;

	theta = theta / PI;
	phi = phi / (2 * PI);
	vec2 irradiance_coords = vec2(phi, theta);
	vec4 irradiance_color = texture(texture_0, irradiance_coords);

	FragColor = irradiance_color;
}

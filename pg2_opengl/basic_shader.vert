#version 460 core

// vertex attributes
layout ( location = 0 ) in vec3 position;
layout ( location = 1 ) in vec3 normal;
layout ( location = 2 ) in vec3 color;

// uniform variables
uniform mat4 P; // Model View Projection

out vec3 world_normal, frag_color;
out vec4 world_position;

void main( void )
{
	world_position = vec4(normal, 1.0f);
	world_normal = normal;
	frag_color = color;
	gl_Position = P * vec4(position, 1.0f); // model-space -> clip-space
}

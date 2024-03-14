#version 460 core

// vertex attributes
layout ( location = 0 ) in vec3 position;
layout ( location = 1 ) in vec3 normal;
// uniform variables
uniform mat4 P;

out vec3 world_normal;

void main( void )
{
	world_normal = normal;
	vec4 pos = P * vec4(position, 1.0f);
	gl_Position = pos.xyww;
}

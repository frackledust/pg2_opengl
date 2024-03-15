#version 460 core

// vertex attributes
layout ( location = 0 ) in vec3 position;
layout ( location = 1 ) in vec3 normal;
layout ( location = 2 ) in vec3 color;

// uniform variables
uniform mat4 _P; // Model View Projection
uniform mat4 _model_matrix;
uniform vec3 _camera_pos;

out vec3 world_normal, frag_color;
out vec4 world_position;
out vec3 camera_pos;

void main( void )
{
	world_position = vec4(normal, 1.0f);
	world_normal = normalize(transpose(inverse(mat3(_model_matrix))) * normal);
	frag_color = color;
	camera_pos = _camera_pos;
	gl_Position = _P * _model_matrix * vec4(position, 1.0f); // model-space -> clip-space
}

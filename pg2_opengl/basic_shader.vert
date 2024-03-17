#version 460 core

// vertex attributes
layout ( location = 0 ) in vec3 _position;
layout ( location = 1 ) in vec3 _normal;
layout ( location = 2 ) in vec3 _color;
layout ( location = 3 ) in vec2 _texcoord;
layout ( location = 4 ) in vec3 _tangent;
layout ( location = 5 ) in int _material_index;


// uniform variables
uniform mat4 _P; // Model View Projection
uniform mat4 _model_matrix;
uniform vec3 _camera_pos;

out vec3 world_normal;
out vec4 world_position;
out vec3 camera_pos;

out vec3 frag_color;
out vec2 texcoord;
out vec3 tangent;
out int material_index;

void main( void )
{
	world_position = vec4(_normal, 1.0f);
	world_normal = normalize(transpose(inverse(mat3(_model_matrix))) * _normal);
	camera_pos = _camera_pos;

	frag_color = _color;
	texcoord = _texcoord;
	tangent = _tangent;
	material_index = _material_index;

	gl_Position = _P * _model_matrix * vec4(_position, 1.0f); // model-space -> clip-space
}

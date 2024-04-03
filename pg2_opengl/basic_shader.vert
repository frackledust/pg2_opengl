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
out mat3 TBN;

void main( void )
{
	world_position = _model_matrix * vec4(_position, 1.0f);
	camera_pos = _camera_pos;

	frag_color = _color;
	texcoord = vec2(_texcoord.x, 1.0f - _texcoord.y);
	tangent = _tangent;
	material_index = _material_index;

	mat3 normalMat = transpose(inverse(mat3(_model_matrix)));
	world_normal = normalize(normalMat * _normal);

	// gram schmidt orthonormalization
	vec3 N = normalize(world_normal);
	tangent = normalize(tangent);
	vec3 T = normalize(tangent - dot(tangent, N) * N);
	vec3 B = normalize(cross(N, T));

	// TBN matrix
	T = normalize(vec3(normalMat * T));
	B = normalize(vec3(normalMat * B));
	N = normalize(vec3(normalMat * N));

	TBN = mat3(T, B, N);

	gl_Position = _P * _model_matrix * vec4(_position, 1.0f); // model-space -> clip-space
}

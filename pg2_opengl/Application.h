#pragma once

#include "objloader.h"
#include "Camera.h"

class Application
{
public:
	GLFWwindow* window;
	GLuint vertex_shader;
	GLuint fragment_shader;
	GLuint shader_program;
	GLuint vao;
	GLuint vbo;

	std::unique_ptr<Camera> camera;
	bool first_mouse = true;
	float last_mouse_x;
	float last_mouse_y;

	int index_count;

	unsigned int irradiance_map_texture_id;

	Application();
	~Application();

	void create_shaders();
	void load_data();
	void check_viewport();
	void load_model();
	void load_textures();
	void run();

	static void cursor_pos_callback(GLFWwindow* window, double mouseXPos, double mouseYPos);
	static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

	struct Vertex
	{
		Vector3 position; /* vertex position */
		Vector3 normal; /* vertex normal */
		Vector3 color; /* vertex color */
		Vector2 texture_coord; /* vertex texture coordinate */
		Vector3 tangent; /* vertex tangent */
		int material_index; /* material index */
	};

	struct TriangleWithAdjacency
	{
		std::array<Vertex, 6> vertices;
	} dst_triangle;
};


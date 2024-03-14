#pragma once

#include "objloader.h"
#include "Camera.h"

class Application
{
public:
	GLFWwindow* window;
	GLuint shader_program;
	GLuint sky_shader_program;
	std::vector<GLuint> shaders;

	GLuint model_vao;
	GLuint model_vbo;
	int model_index_count;

	GLuint sky_vao;
	GLuint sky_vbo;
	int sky_index_count;

	std::unique_ptr<Camera> camera;
	bool first_mouse = true;
	float last_mouse_x;
	float last_mouse_y;

	Application();
	~Application();

	void create_shaders(std::string name, GLuint& active_shader_program);
	void load_data();
	void check_viewport();
	void load_model(std::string file_name, GLuint& vao, GLuint& vbo, int& index_count);
	void load_textures();
	void draw_skydome();
	void load_skydome_texture();
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


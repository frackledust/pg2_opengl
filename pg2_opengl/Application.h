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
	void create_bindless_material(const MaterialLibrary& materials);
	void load_model(std::string file_name, GLuint& vao, GLuint& vbo, int& index_count, bool bindless);
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

#pragma pack( push, 1 ) // 1 B alignment
	struct GLMaterial
	{
		Color3f diffuse; // 3 * 4 B
		GLbyte pad0[4]; // + 4 B = 16 B
		GLuint64 tex_diffuse_handle{ 0 }; // 1 * 8 B
		GLbyte pad1[8]; // + 8 B = 16 B
		Color3f rma; // 3 * 4 B
		GLbyte pad2[4]; // + 4 B = 16 B
		GLuint64 tex_rma_handle{ 0 }; // 1 * 8 B
		GLbyte pad3[8]; // + 8 B = 16 B
		Color3f normal; // 3 * 4 B
		GLbyte pad4[4]; // + 4 B = 16 B
		GLuint64 tex_normal_handle{ 0 }; // 1 * 8 B
		GLbyte pad5[8]; // + 8 B = 16 B
	};
#pragma pack( pop )


};


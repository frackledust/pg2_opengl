#include "pch.h"
#include "Application.h"
#include "pch.h"
#include "tutorials.h"
#include "utils.h"
#include "glutils.h"
#include "matrix4x4.h"
#include "color.h"
#include "texture.h"
#include "objloader.h"
#include <texture.h>
#include <iostream>

void my_glfw_callback(const int error, const char* description)
{
	printf("GLFW Error (%d): %s\n", error, description);
}

void Application::cursor_pos_callback(GLFWwindow* window, double mouseXPos, double mouseYPos) {
	Application* app = (Application*)glfwGetWindowUserPointer(window);
	if(app->first_mouse)
	{
		app->last_mouse_x = mouseXPos;
		app->last_mouse_y = mouseYPos;
		app->first_mouse = false;
	}
	float x_offset = mouseXPos - app->last_mouse_x;
	float y_offset = mouseYPos - app->last_mouse_y;

	app->last_mouse_x = mouseXPos;
	app->last_mouse_y = mouseYPos;

	float sensitivity = 0.3f;
	x_offset *= sensitivity;
	y_offset *= sensitivity;
	
	app->camera->change_angles(x_offset, y_offset);
}

void Application::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	Application* app = (Application*) glfwGetWindowUserPointer(window);

	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	// Change camera position
	const float camera_speed = 0.05f;
	bool updated = false;
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		// Front
		app->camera->eye += camera_speed * app->camera->view_dir;
		updated = true;
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS){
		// Back
		app->camera->eye -= camera_speed * app->camera->view_dir;
		updated = true;
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		// Left
		Vector3 direction = app->camera->view_dir.CrossProduct(app->camera->up);
		direction.Normalize();
		app->camera->eye -= camera_speed * direction;
		updated = true;
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		// Right
		Vector3 direction = app->camera->view_dir.CrossProduct(app->camera->up);
		direction.Normalize();
		app->camera->eye += camera_speed * direction;
		updated = true;
	}
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
		// Up
		app->camera->eye += camera_speed * app->camera->up;
		updated = true;
	}
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
		// Down
		app->camera->eye -= camera_speed * app->camera->up;
		updated = true;
	}

	if(updated)
	{
		std::cout << app->camera->eye.data[0] << " " << app->camera->eye.data[1] << " " << app->camera->eye.data[2] << std::endl;
	}

	// printf("key_callback [%d,%d,%d,%d] \n", key, scancode, action, mods);
}


Application::Application()
{
	int width = 1600;
	int height = 1600;
	camera = std::make_unique<Camera>();

	glfwSetErrorCallback(my_glfw_callback);

	if (!glfwInit())
	{
		std::cout << "Error";
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_SAMPLES, 8);
	glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
	glfwWindowHint(GLFW_DOUBLEBUFFER, GL_TRUE);
	
	this->window = glfwCreateWindow(width, height, "PG2 OpenGL", nullptr, nullptr);
	if (!window)
	{
		glfwTerminate();
		std::cout << "Error";
	}
	glfwSetWindowUserPointer(window, this);

	glfwSetCursorPosCallback(window, Application::cursor_pos_callback);
	glfwSetKeyCallback(window, Application::key_callback);
	//glfwSetWindowSizeCallback(window.get(), [](GLFWwindow* window, int width, int height) { Application::get_instance()->window_size_callback(window, width, height); });
	//glfwSetWindowFocusCallback(window.get(), [](GLFWwindow* window, int focused) { Application::get_instance()->window_focus_callback(window, focused); });
	//glfwSetWindowIconifyCallback(window.get(), [](GLFWwindow* window, int iconified) { Application::get_instance()->window_iconify_callback(window, iconified); });

	glfwSetFramebufferSizeCallback(window, framebuffer_resize_callback);
	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		if (!gladLoadGL())
		{
			std::cout << "Error";
		}
	}

	glEnable(GL_DEBUG_OUTPUT);
	glDebugMessageCallback(gl_callback, nullptr);

	printf("OpenGL %s, ", glGetString(GL_VERSION));
	printf("%s", glGetString(GL_RENDERER));
	printf(" (%s)\n", glGetString(GL_VENDOR));
	printf("GLSL %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));

	glEnable(GL_MULTISAMPLE);

	// map from the range of NDC coordinates <-1.0, 1.0>^2 to <0, width> x <0, height>
	glViewport(0, 0, width, height);
	// GL_LOWER_LEFT (OpenGL) or GL_UPPER_LEFT (DirectX, Windows) and GL_NEGATIVE_ONE_TO_ONE or GL_ZERO_TO_ONE
	glClipControl(GL_UPPER_LEFT, GL_NEGATIVE_ONE_TO_ONE);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_STENCIL_TEST);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
}

Application::~Application()
{
	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader);
	glDeleteProgram(shader_program);

	delete window;
	glfwTerminate();
}

void Application::create_shaders(){
	this->vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	std::vector<char> shader_source;
	if (LoadShader("basic_shader.vert", shader_source) == S_OK)
	{
		const char* tmp = static_cast<const char*>(&shader_source[0]);
		glShaderSource(vertex_shader, 1, &tmp, nullptr);
		glCompileShader(vertex_shader);
	}
	CheckShader(vertex_shader);

	this->fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
	if (LoadShader("basic_shader.frag", shader_source) == S_OK)
	{
		const char* tmp = static_cast<const char*>(&shader_source[0]);
		glShaderSource(fragment_shader, 1, &tmp, nullptr);
		glCompileShader(fragment_shader);
	}
	CheckShader(fragment_shader);

	this->shader_program = glCreateProgram();
	glAttachShader(shader_program, vertex_shader);
	glAttachShader(shader_program, fragment_shader);
	glLinkProgram(shader_program);

	glUseProgram(shader_program);
}

void Application::load_data() {
	Texture3f image = Texture3f("../../../data/denoise/color_2spp.exr"); // linear HDR image
	const int width = image.width();
	const int height = image.height();

	std::cout << width;

	this->index_count = 3;

	// setup vertex buffer as AoS (array of structures)
	GLfloat vertices[] =
	{
		-0.9f, 0.9f, 0.0f,  0.0f, 1.0f, // vertex 0 : p0.x, p0.y, p0.z, t0.u, t0.v
		0.9f, 0.9f, 0.0f,   1.0f, 1.0f, // vertex 1 : p1.x, p1.y, p1.z, t1.u, t1.v
		0.0f, -0.9f, 0.0f,  0.5f, 0.0f  // vertex 2 : p2.x, p2.y, p2.z, t2.u, t2.v
	};
	const int no_vertices = 3;
	const int vertex_stride = sizeof(vertices) / no_vertices;
	// optional index array
	unsigned int indices[] =
	{
		0, 1, 2
	};

	this->vao = 0;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	this->vbo = 0;
	glGenBuffers(1, &vbo); // generate vertex buffer object (one of OpenGL objects) and get the unique ID corresponding to that buffer
	glBindBuffer(GL_ARRAY_BUFFER, vbo); // bind the newly created buffer to the GL_ARRAY_BUFFER target
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); // copies the previously defined vertex data into the buffer's memory
	// vertex position
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, vertex_stride, 0);
	glEnableVertexAttribArray(0);
	// vertex texture coordinates
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, vertex_stride, (void*)(sizeof(float) * 3));
	glEnableVertexAttribArray(1);

	GLuint ebo = 0; // optional buffer of indices
	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
}

void Application::load_model() {
	//std::string file_name = "../../../data/adjacent_triangles.obj";
	std::string file_name = "../../../data/piece_02/piece_02.obj";
	SceneGraph scene;
	MaterialLibrary materials;

	LoadOBJ(file_name, scene, materials);

	std::vector<TriangleWithAdjacency> triangles;

	for (SceneGraph::iterator iter = scene.begin(); iter != scene.end(); ++iter)
	{
		const std::string& node_name = iter->first;
		const auto& node = iter->second;

		const auto& mesh = std::static_pointer_cast<Mesh>(node);

		if (mesh)
		{
			for (Mesh::iterator iter = mesh->begin(); iter != mesh->end(); ++iter)
			{
				const auto& src_triangle = Triangle3i(**iter);
				std::shared_ptr<Material> material = iter.triangle_material();
				const int material_index = int(std::distance(std::begin(materials), materials.find(material->name())));

				printf("Triangle:\n");

				for (int i = 0; i < 3; ++i)
				{
					dst_triangle.vertices[i * 2].position = src_triangle.position(i);
					dst_triangle.vertices[i * 2].position.Print();
					dst_triangle.vertices[i * 2].normal = src_triangle.normal(i);
					dst_triangle.vertices[i * 2].color = Vector3(1.0f, 1.0f, 1.0f);
					dst_triangle.vertices[i * 2].texture_coord = Vector2(src_triangle.texture_coord(i).x, src_triangle.texture_coord(i).y);
					dst_triangle.vertices[i * 2].tangent = src_triangle.tangent(i);
					dst_triangle.vertices[i * 2].material_index = material_index;

					dst_triangle.vertices[i * 2 + 1].position = src_triangle.adjacent_vertex_position(i).value_or(Vector3());
					dst_triangle.vertices[i * 2 + 1].normal = Vector3();
					dst_triangle.vertices[i * 2 + 1].color = Vector3();
					dst_triangle.vertices[i * 2 + 1].texture_coord = Vector2();
					dst_triangle.vertices[i * 2 + 1].tangent = Vector3();
					dst_triangle.vertices[i * 2 + 1].material_index = -1;
				}

				triangles.push_back(dst_triangle);


				printf("Adjacent vertices:\n");

				for (int i = 0; i < 3; ++i)
				{
					std::optional<Vector3> av = src_triangle.adjacent_vertex_position(i);
					if (av.has_value())
					{
						av.value().Print();
					}
					else
					{
						printf("-\n");
					}
				}
			}
		}
	}

	// Bind to VAO
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glBindVertexArray(vao);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, triangles.size() * sizeof(TriangleWithAdjacency), &triangles[0], GL_STATIC_DRAW);

	int stride = sizeof(Vertex) * 2;
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (GLvoid*)(0));
	glEnableVertexAttribArray(0); // Vertex position attribute

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)offsetof(Vertex, normal));
	glEnableVertexAttribArray(1); // Vertex normal attribute

	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride, (void*)offsetof(Vertex, color));
	glEnableVertexAttribArray(2); // Vertex color attribute

	glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, stride, (void*)offsetof(Vertex, texture_coord));
	glEnableVertexAttribArray(3); // Texture coordinates attribute

	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, stride, (void*)offsetof(Vertex, tangent));
	glEnableVertexAttribArray(4); // Vertex tangent attribute

	glVertexAttribPointer(5, 1, GL_INT, GL_FALSE, stride, (void*)offsetof(Vertex, material_index));
	glEnableVertexAttribArray(5); // Material index attribute



	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(vao);

	GLuint err = glGetError();
	if (err != GL_NO_ERROR)
	{
		std::cout << "GL ERROR: " << err << std::endl;
		return;
	}
	index_count = triangles.size() * 6;
}

void Application::check_viewport() {

	// get view and projection matrix
	Matrix4x4 Tx = {
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1
	};

	Tx = camera->get_view_matrix() * Tx;
	Tx = camera->get_projection_matrix() * Tx;
	//Tx = Tx * camera->get_viewport_matrix();

	SetMatrix4x4(this->shader_program, Tx.data(), "P");
}

void Application::load_textures() {
	Texture3f map = Texture3f("../../../data/maps/lebombo_irradiance_map.exr"); // linear HDR image
	Color3f* data = map.data();

	if (data)
	{
		glGenTextures(1, &irradiance_map_texture_id);
		glBindTexture(GL_TEXTURE_2D, irradiance_map_texture_id);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, map.width(), map.height(), 0, GL_RGB, GL_FLOAT, data);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}
	else
	{
		fprintf(stderr, "Failed to load HDR image.");
		exit(EXIT_FAILURE);
	}
}

void Application::run()
{
	load_model();
	load_textures();
	//load_data();
	create_shaders();

	// main loop
	while (!glfwWindowShouldClose(window))
	{
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f); // state setting function
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT); // state using function

		check_viewport();

		glBindVertexArray(vao);
		glDrawArrays(GL_TRIANGLES, 0, index_count);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glDeleteBuffers(1, &vbo);
	glDeleteVertexArrays(1, &vao);
}
#include "Logger.h"
#include "utils.h"
#define GLEW_STATIC
#include "GL/glew.h"
#include "GLFW/glfw3.h"
#pragma once

class RenderObject
{

};

class GraphicsEngine
{
public:
	GraphicsEngine();
	void update(float dt);
	bool is_running() const;
	int add_object(RenderObject object);
	void remove_object(int id);
	RenderObject create_object(CreateRenderObjectRequest request);
	void stop();
	GLuint VertexArrayID;
	GLuint VBO;
	GLuint shaderProgram;
	void init_window();
	
private:
	std::atomic<bool> running;
	Logger logger;
	std::vector<RenderObject> objects;
	std::unordered_map<int, size_t> id2index;


	int window_width;
	int window_height;
	bool fullscreen;
	float aspect;
	GLFWwindow *window;
	

};
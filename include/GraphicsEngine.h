#include "Logger.h"
#include "utils.h"
#define GLEW_STATIC
#include "GL/glew.h"
#include "GLFW/glfw3.h"
#pragma once

class RenderObject
{
public:
	int id;
	glm::vec2 pos;
	glm::vec2 size;
	void update();
	glm::mat2 translation;
	glm::mat2 rotation;
	glm::mat2 scale;
	glm::mat2 view_projection;

private:
	GLuint VAO;
	GLuint VBO;
	GLuint shaderProgram;
	GLuint FBO;
	GLuint texture;
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
	void init_window();
	
private:
	std::atomic<bool> running;
	Logger logger;
	std::vector<RenderObject> objects;
	std::unordered_map<int, size_t> id2index;
	int next_id = 0;


	glm::vec2 camera_pos;
	float zoom;
	int window_width;
	int window_height;
	bool fullscreen;
	float aspect;
	GLFWwindow *window;
	

};
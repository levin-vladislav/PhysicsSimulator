#include "Logger.h"
#include "utils.h"
#define GLEW_STATIC
#include "GL/glew.h"
#include "GLFW/glfw3.h"
#pragma once

class RenderObject
{
public:
	RenderObject(int id, glm::vec2 pos, std::vector<float> vertices,
		GLuint shaderProgram, GLuint modelLoc, GLFWwindow* window);
	int id;
	glm::vec2 pos;
	glm::vec2 size;
	void update();
	glm::vec2 translation;
	float rotation;
	float scale;
	std::vector<float> vertices;

private:
	GLuint VAO;
	GLuint VBO;
	GLuint shaderProgram;
	GLuint FBO;
	GLuint texture;
	GLFWwindow* window;
	GLuint modelLoc;
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
	void init_shaders();
	void init_grid();
	void moveCamera(glm::vec2 step);
	void setCameraPos(glm::vec2 pos);
	void setZoom(float zoom);
	void Zoom(float zoom);
	void setGrid(float gridSize, float gridThickness);
	std::queue<CreateRenderObjectRequest> objectQueue;

private:
	std::atomic<bool> running;
	Logger logger;
	std::vector<RenderObject> objects;
	std::unordered_map<int, size_t> id2index;
	int next_id = 0;

	std::atomic<bool> can_update = true;
	glm::vec2 camera_pos;
	float zoom = 1.0f;
	int window_width;
	int window_height;
	bool fullscreen;
	float aspect;
	float gridSize = 0.2f;
	float gridThickness = 0.01f;
	glm::mat4 view = glm::mat4(1.0f);
	
	GLFWwindow* window;
	GLuint bodyShaderProgram;
	GLuint gridShaderProgram;
	GLuint viewLoc;
	GLuint modelLoc;
	GLuint gridThicknessLoc;
	GLuint gridViewLoc;
	GLuint gridModelLoc;
	GLuint gridSizeLoc;
	GLuint gridVAO;
	GLuint gridVBO;

};

GLuint compileShader(GLenum type, const char* source);
GLuint createProgram(const char* vertexPath, const char* fragmentPath);

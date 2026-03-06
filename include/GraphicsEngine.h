#include "Logger.h"
#include "utils.h"
#define GLEW_STATIC
#include "GL/glew.h"
#include "GLFW/glfw3.h"
#pragma once

// class that stores information of renderable objects
class RenderObject
{
public:
	RenderObject(int id, glm::vec2 pos, std::vector<float> vertices,
		GLuint shaderProgram, GLuint modelLoc, GLFWwindow* window, int shape = 0, float radius = 1.0f);
	int id;
	glm::vec2 pos;
	// glm::vec2 size;
	void update();
	glm::vec2 translation;
	float rotation;
	float radius;
	std::vector<float> vertices;
	int shape;

private:
	GLuint VAO;
	GLuint VBO;
	GLuint shaderProgram;
	// GLuint FBO;
	// GLuint texture;
	GLFWwindow* window;
	GLuint modelLoc;
};

// Class managing graphics
class GraphicsEngine
{
public:
	GraphicsEngine();
	void update(float dt);
	bool is_running() const;
	int add_object(RenderObject object);
	// void remove_object(int id);
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

	std::vector<RenderObject> objects;

private:
	std::atomic<bool> running;
	Logger logger;
	std::unordered_map<int, size_t> id2index;
	int next_id = 0;

	std::atomic<bool> can_update = true;
	glm::vec2 camera_pos = glm::vec2(0.0f);
	float zoom = 1.0f;
	int window_width = 800;
	int window_height = 600;
	bool fullscreen = false;
	float aspect = (float)window_width / (float)window_height;
	float gridSize = 0.2f;
	float gridThickness = 0.01f;
	glm::mat4 view = glm::mat4(1.0f);
	
	GLFWwindow* window;
	GLuint bodyShaderProgram;
	GLuint gridShaderProgram;
	GLuint viewLoc;
	GLuint modelLoc;
	GLuint radiusLoc;
	GLuint shapeLoc;

	GLuint gridThicknessLoc;
	GLuint gridViewLoc;
	GLuint gridModelLoc;
	GLuint gridSizeLoc;
	GLuint gridVAO;
	GLuint gridVBO;



};

GLuint compileShader(GLenum type, const char* source);
GLuint createProgram(const char* vertexPath, const char* fragmentPath);

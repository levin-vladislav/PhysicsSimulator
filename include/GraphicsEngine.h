#include "Logger.h"
#include "utils.h"
#define GLEW_STATIC
#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include "imgui.h"
//#include "Dispatcher.h"
#pragma once

class Dispatcher;

struct CreateBodyContext
{
	float x = 0.0f; 
	float y = 0.0f; 
	float m = 1.0f;
	int shape = 0;
	
};

class IWidget
{
public:
	virtual void draw() = 0;
	virtual ~IWidget() = default;
};

class InputWidget : public IWidget
{
	Logger logger;
	char inputBuffer[256];
public:
	InputWidget();
	void draw() override;
};

class BodyInspectorWidget : public IWidget
{
public:
	CreateBodyContext ctx = {};
	bool show = false;
	int id = 1;
	Dispatcher* ptr;
	BodyInspectorWidget(Dispatcher* ptr);
	void draw() override;
};

class WorldInspectorWidget : public IWidget
{
public:
	Dispatcher* ptr;
	WorldInspectorWidget(Dispatcher* ptr);
	void draw() override;
};

class BodyListWidget : public IWidget
{
public:
	bool show = false;
	int id = 1;
	Dispatcher* ptr;
	BodyInspectorWidget* inspector_ptr;
	BodyListWidget(Dispatcher* ptr, BodyInspectorWidget* inspector_ptr);
	void draw() override;
};


class GuiManager
{
public:
	void init(Dispatcher* ptr);
	void draw();
	std::vector<IWidget*> widgets;
};

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
	void init_window(Dispatcher* ptr);
	void init_shaders();
	void init_grid();
	void moveCamera(glm::vec2 step);
	void setCameraPos(glm::vec2 pos);
	static void setZoom(float zoom);
	static void Zoom(float zoom);
	
	void setGrid(float gridSize, float gridThickness);
	std::queue<CreateRenderObjectRequest> objectQueue;

	std::vector<RenderObject> objects;

	static void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);

	static float zoom;

	glm::vec2 clickedCoordinate;


	std::atomic<bool> click;

private:
	GuiManager gui;
	std::atomic<bool> running;
	Logger logger;
	std::unordered_map<int, size_t> id2index;
	int next_id = 0;

	std::atomic<bool> can_update = true;
	glm::vec2 camera_pos = glm::vec2(0.0f);
	int window_width = 800;
	int window_height = 600;
	bool fullscreen = false;
	float aspect = (float)window_width / (float)window_height;
	float gridSize = 0.2f;
	float gridThickness = 0.01f;
	glm::mat4 view = glm::mat4(1.0f);

	void updateCamera();

	glm::vec2 prevMousePos = glm::vec2(0.0f,0.0f);

	
	bool isClicked;
	GLFWwindow* window;
	GLFWwindow* GUIwindow;
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

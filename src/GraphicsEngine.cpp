#include "GraphicsEngine.h"
#include <iostream>

GraphicsEngine::GraphicsEngine() : running(false), logger(Logger("GraphicsEngine")), 
									window_width(800), window_height(600), fullscreen(true)
{
	if (!glfwInit())
	{
		logger.error("Couldn't initialize GLFW. Quitting...");
		stop();
	}

	logger.info("Initialized.");
	running.store(true);
}

bool GraphicsEngine::is_running() const
{
	return running.load();
}

RenderObject::RenderObject(int id, glm::vec2 pos, std::vector<float> vertices, const char* vxShader, const char* fgShader, GLFWwindow* window)
{
    this->vertices = vertices;
    this->window = window;
    std::cout << "RENDEROBJECT THREAD: "
        << std::this_thread::get_id() << std::endl;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    std::cout << "2. Window: " << window << std::endl;
    glfwMakeContextCurrent(window);
    std::cout << "3. Current Context: " << glfwGetCurrentContext() << std::endl;
    if (!glfwGetCurrentContext()) std::cerr << "render object created without context" << std::endl;
    std::cout << "vx" << vxShader << std::endl;
    std::cout << "fg" << fgShader << std::endl;
    shaderProgram = createProgram(vxShader, fgShader);
    
    this->id = id;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    //glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
    
    //glVertexAttribPointer(0, 2, 
    //                      GL_FLOAT, GL_FALSE, 
    //                    2 * sizeof(float), (void*)0);
    
    glBufferData(GL_ARRAY_BUFFER, vertices.size()*sizeof(float), vertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    glBindVertexArray(0);
    glfwMakeContextCurrent(0);

}

RenderObject GraphicsEngine::create_object(CreateRenderObjectRequest request)
{
    std::cout << "1. Current Context: " << window << std::endl;
    return RenderObject(next_id++, 
        request.pos, request.vertices,
        request.vertexShaderPath, request.fragmentShaderPath,
        window);
}

void RenderObject::update()
{   
    glUseProgram(shaderProgram);
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLE_FAN, 0, vertices.size());
}

void GraphicsEngine::update(float dt)
{
    while (!objectQueue.empty())
    {
        add_object(create_object(objectQueue.front()));
        objectQueue.pop();

    }

    glfwMakeContextCurrent(window);
    if (!running.load()) return;

    if (glfwWindowShouldClose(window)) {
        stop();
        return;
    }
    
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    if (can_update.load())
    {
        for (RenderObject& obj : objects)
            {
                obj.update();
            }
    }
  

    glfwSwapBuffers(window);
    glfwPollEvents();
    glfwMakeContextCurrent(0);
}

void GraphicsEngine::init_window()
{
    std::cout << "WINDOW THREAD: "
        << std::this_thread::get_id() << std::endl;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(window_width, window_height,
        "Physics Simulation Engine", NULL, NULL);

    if (!window) {
        logger.error("Failed to create window!");
        glfwTerminate();
        return;
    }

    glfwMakeContextCurrent(window);

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        logger.error("Failed to initialize GLEW!");
        return;
    }

    glViewport(0, 0, window_width, window_height);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glfwMakeContextCurrent(0);
    
}
void GraphicsEngine::stop()
{
	glfwTerminate();
	running.store(false);
}

int GraphicsEngine::add_object(RenderObject object)
{
    can_update.store(false);
    int id = next_id++;
    id2index[id] = objects.size();
    object.id = id;
    objects.push_back(object);
    logger.info(std::format("Object {} is rendered.", id));
    can_update.store(true);
    return id;
}

GLuint compileShader(GLenum type, const char* source) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);

    // Check for compilation errors
    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        std::cerr << "SHADER COMPILE ERROR:\n" << infoLog << std::endl;
    }
    return shader;
}

GLuint createProgram(const char* vertexPath, const char* fragmentPath) {
    std::string vShaderStr = readFile(vertexPath);
    std::string fShaderStr = readFile(fragmentPath);
    const char* vShaderCode = vShaderStr.c_str();
    const char* fShaderCode = fShaderStr.c_str();

    std::cout << vShaderCode << std::endl;
    std::cout << fShaderCode << std::endl;

    GLuint vertexShader = compileShader(GL_VERTEX_SHADER, vShaderCode);
    GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, fShaderCode);

    GLuint program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);

    // Check for linking errors
    GLint success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(program, 512, NULL, infoLog);
        std::cerr << "PROGRAM LINK ERROR:\n" << infoLog << std::endl;
    }

    // Clean up shaders
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    return program;
}


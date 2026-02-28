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

RenderObject::RenderObject(int id, glm::vec2 pos, std::vector<float> vertices, 
    GLuint shaderProgram, GLuint modelLoc, GLFWwindow* window)
{
    this->vertices = vertices;
    this->window = window;
    this->pos = pos;
    this->shaderProgram = shaderProgram;
    this->modelLoc = modelLoc;
    this->id = id;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    std::cout << "Vertices: ";
    for (float element : vertices) { // or auto element
        std::cout << element << " ";
    }
    std::cout << std::endl; // Print a newline at the end
    //glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
    
    //glVertexAttribPointer(0, 2, 
    //                      GL_FLOAT, GL_FALSE, 
    //                    2 * sizeof(float), (void*)0);
    
    glBufferData(GL_ARRAY_BUFFER, vertices.size()*sizeof(float), vertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);

}

RenderObject GraphicsEngine::create_object(CreateRenderObjectRequest request)
{
    std::cout << "1. Current Context: " << window << std::endl;
    std::cout << "Shader: " << bodyShaderProgram << std::endl;

    return RenderObject(request.id,
        request.pos, request.vertices,
        bodyShaderProgram,
        modelLoc,
        window);
}

void RenderObject::update()
{   
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(pos, 0.0f));
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}

void GraphicsEngine::update(float dt)
{
    glfwMakeContextCurrent(window);
    glUseProgram(bodyShaderProgram);
    while (!objectQueue.empty())
    {
        add_object(create_object(objectQueue.front()));
        objectQueue.pop();

    }
       
    if (!running.load()) return;

    if (glfwWindowShouldClose(window)) {
        stop();
        return;
    }

    glfwGetWindowSize(window, &window_width, &window_height);
    aspect = (float)window_width / (float)window_height;
    glViewport(0, 0, window_width, window_height);
    view = glm::translate(glm::ortho(-aspect, aspect, -1.0f, 1.0f), glm::vec3(-camera_pos, 0.0f));
    view = glm::scale(view, glm::vec3(zoom, zoom, 1.0f));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    if (can_update.load())
    {
        glUseProgram(bodyShaderProgram);
        for (RenderObject& obj : objects)
            {
                obj.update();
            }
    }
  

    glfwSwapBuffers(window);
    glfwPollEvents();
    glfwMakeContextCurrent(0);
}

void GraphicsEngine::setCameraPos(glm::vec2 pos)
{
    camera_pos = pos;
}

void GraphicsEngine::moveCamera(glm::vec2 step)
{
    camera_pos = camera_pos + step;
}

void GraphicsEngine::setZoom(float zoom)
{
    this->zoom = zoom;
}

void GraphicsEngine::Zoom(float zoom)
{
    this->zoom *= zoom;
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

    std::cout << "Window: " << window << std::endl;

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        logger.error("Failed to initialize GLEW!");
        return;
    }

    glfwGetWindowSize(window, &window_width, &window_height);
    aspect = (float)window_width / (float)window_height;
    glViewport(0, 0, window_width, window_height);
    view = glm::ortho(-aspect, aspect, -1.0f, 1.0f) * view;
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    
}

void printProgramInfoLog(GLuint program) {
    int max_length = 0;
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &max_length);

    if (max_length > 0) {
        std::vector<char> info_log(max_length);
        int actual_length = 0;
        glGetProgramInfoLog(program, max_length, &actual_length, info_log.data());
        std::cout << "Program Info Log:" << std::endl << info_log.data() << std::endl;
    }
}

void GraphicsEngine::init_shaders()
{
    bodyShaderProgram = createProgram("shaders\\body.vx", "shaders\\body.fg");
    glValidateProgram(bodyShaderProgram);
    //logger.info(std::format("viewLoc: {}", viewLoc));
    logger.info(std::format("modelLoc: {}", modelLoc));
    GLint params = -1;
    glGetProgramiv(bodyShaderProgram, GL_VALIDATE_STATUS, &params);
    if (GL_TRUE != params)
    {
        std::cerr << "Program validation failed!" << std::endl;
        printProgramInfoLog(bodyShaderProgram);
    }
    viewLoc = glGetUniformLocation(bodyShaderProgram, "view");
    modelLoc = glGetUniformLocation(bodyShaderProgram, "model");
    
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

}

void GraphicsEngine::stop()
{
	glfwTerminate();
	running.store(false);
}

int GraphicsEngine::add_object(RenderObject object)
{
    can_update.store(false);
    int id = object.id;
    id2index[id] = objects.size();
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


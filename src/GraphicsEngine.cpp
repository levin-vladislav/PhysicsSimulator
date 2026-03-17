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
    GLuint shaderProgram, GLuint modelLoc, GLFWwindow* window, int shape, float radius)
{
    this->vertices = vertices;
    this->window = window;
    this->pos = pos;
    this->shaderProgram = shaderProgram;
    this->modelLoc = modelLoc;
    this->id = id;
    this->shape = shape;
    this->radius = radius;
    rotation = 0.0f;
    translation = glm::vec2(0.0f);

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
        
    glBufferData(GL_ARRAY_BUFFER, vertices.size()*sizeof(float), vertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
}

RenderObject GraphicsEngine::create_object(CreateRenderObjectRequest request)
{

    return RenderObject(request.id,
        request.pos, request.vertices,
        bodyShaderProgram,
        modelLoc,
        window,
        request.shape,
        request.radius);
}

void RenderObject::update()
{   
    glUseProgram(shaderProgram);
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(pos, 0.0f));
    model = glm::rotate(model, rotation, glm::vec3(0.0f, 0.0f, 1.0f));
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}

void GraphicsEngine::update(float dt)
{
    glfwMakeContextCurrent(window);
    glfwGetWindowSize(window, &window_width, &window_height);
    aspect = (float)window_width / (float)window_height;
    glViewport(0, 0, window_width, window_height);
    
    view = glm::translate(glm::ortho(-aspect, aspect, -1.0f, 1.0f), glm::vec3(-camera_pos, 0.0f)*zoom);
    view = glm::scale(view, glm::vec3(zoom, zoom, 1.0f));


    
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

    
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(gridShaderProgram);
    glUniformMatrix4fv(gridViewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniform1f(gridThicknessLoc, gridThickness);
    glUniform1f(gridSizeLoc, gridSize);
    glm::mat4 gridModel = glm::mat4(1.0f);
    gridModel = glm::translate(gridModel, glm::vec3(round(camera_pos.x), round(camera_pos.y), 0.0f));
    glUniformMatrix4fv(gridModelLoc, 1, GL_FALSE, glm::value_ptr(gridModel));
    glBindVertexArray(gridVAO);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

    if (can_update.load())
    {
        
        for (RenderObject& obj : objects)
            {
                glUseProgram(bodyShaderProgram);
                glUniform1i(shapeLoc, obj.shape);
                glUniform1f(radiusLoc, obj.radius);
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

void GraphicsEngine::setGrid(float gridSize, float gridThickness)
{
    this->gridSize = gridSize;
    this->gridThickness = gridThickness;
}

void GraphicsEngine::init_window()
{
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

    glfwGetWindowSize(window, &window_width, &window_height);
    aspect = (float)window_width / (float)window_height;
    glViewport(0, 0, window_width, window_height);
    view = glm::ortho(-aspect, aspect, -1.0f, 1.0f) * view;
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);

}

void printProgramInfoLog(GLuint program) {
    int max_length = 0;
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &max_length);

    if (max_length > 0) {
        std::vector<char> info_log(max_length);
        int actual_length = 0;
        glGetProgramInfoLog(program, max_length, &actual_length, info_log.data());
        
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

    shapeLoc = glGetUniformLocation(bodyShaderProgram, "shape");
    radiusLoc = glGetUniformLocation(bodyShaderProgram, "radius");

    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

    gridShaderProgram = createProgram("shaders\\grid.vx", "shaders\\grid.fg");
    gridViewLoc = glGetUniformLocation(gridShaderProgram, "view");
    gridThicknessLoc = glGetUniformLocation(gridShaderProgram, "gridThickness");
    gridSizeLoc = glGetUniformLocation(gridShaderProgram, "gridSize");
    gridModelLoc = glGetUniformLocation(gridShaderProgram, "model");

    
    }

void GraphicsEngine::init_grid()
{
    glfwMakeContextCurrent(window);
    glGenVertexArrays(1, &gridVAO);
    glGenBuffers(1, &gridVBO);
    glBindVertexArray(gridVAO);
    glBindBuffer(GL_ARRAY_BUFFER, gridVBO);
    float vertices[] = { -20.0f, -20.0f, 20.0f, -20.0f, 20.0f, 20.0f, -20.0f, 20.0f };
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
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


#include "GraphicsEngine.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <cmath>
#include <iostream>
#include "Dispatcher.h"

float GraphicsEngine::zoom = 1.0f;

GraphicsEngine::GraphicsEngine() :  running(false), logger(Logger("GraphicsEngine")), 
									window_width(800), window_height(600), fullscreen(true)
{
	if (!glfwInit())
	{
		logger.error("Couldn't initialize GLFW. Quitting...");
		stop();
	}
    gui = GuiManager();
	logger.info("Initialized.");
    click.store(false);
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

    glfwMakeContextCurrent(window);

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

InputWidget::InputWidget() : logger(Logger("Input widget"))
{

    inputBuffer[0] = '\0';
}

void InputWidget::draw()
{
    ImVec2 displaySize = ImGui::GetIO().DisplaySize;
    ImGui::SetNextWindowPos(ImVec2(0, displaySize.y - 120));
    ImGui::SetNextWindowSize(ImVec2(displaySize.x, 120));
    ImGui::Begin("Input");
    
    ImGui::InputText("Input", inputBuffer, 256);
    if (ImGui::Button("execute"))
    {
        logger.info(std::string(inputBuffer));
        inputBuffer[0] = '\0';
    }

    ImGui::End();
}

WorldInspectorWidget::WorldInspectorWidget(Dispatcher* ptr)
{
    this->ptr = ptr;
}

void WorldInspectorWidget::draw()
{
    ImGui::Begin("WorldInspector");
    if (ImGui::Button("toggle"))
    {
        ptr->engine_ptr->physicsEngine.toggle();
    }
    ImGui::End();
}

BodyInspectorWidget::BodyInspectorWidget(Dispatcher* ptr)
{
    this->ptr = ptr;
}

void BodyInspectorWidget::draw()
{
    const char* items[] = { "rectangle", "circle" };
    ImVec2 displaySize = ImGui::GetIO().DisplaySize;
    if (show)
    {
        ImGui::SetNextWindowSize(ImVec2(displaySize.x, 280));
    }
    else
    {
        ImGui::SetNextWindowSize(ImVec2(displaySize.x, 240));
    }
    ImGui::Begin("BodyInspector");

    if (ImGui::Button("+"))
    {
        show = true;
    }

    if (show)
    {
        ImGui::Text("pos:");
        ImGui::SameLine();
        bool x = ImGui::InputFloat("x##0", &ctx.x, 0.0f, 0.0f, "%.2f", 0);
        ImGui::SameLine();
        bool y = ImGui::InputFloat("y##0", &ctx.y, 0.0f, 0.0f, "%.2f", 0);
        bool m = ImGui::InputFloat("mass", &ctx.m, 0.0f, 0.0f, "%.2f", 0);
        ImGui::Combo("Shape", &ctx.shape, items, 2);
        float r = 1.0f;
        if (ImGui::Button("Add"))
        {
            ptr->engine_ptr->request(CreateBodyRequest{
                            glm::vec2(ctx.x, ctx.y),
                            glm::vec2(0.0f, 0.0f),
                            ctx.m,
                            ctx.shape,
                            r / 2 },
                            CreateRenderObjectRequest{
                                glm::vec2(ctx.x, ctx.y),
                                std::vector<float> {-r / 2, -r / 2,
                                                    r / 2, -r / 2,
                                                     r / 2, r / 2,
                                                     -r / 2, r / 2},
                                ctx.shape,
                                r / 2
                }

            );
            ctx = {};
            show = false;
        }
    }


    if (ptr->engine_ptr->physicsEngine.get_bodies().size() > 0)
    {
        float x = round(ptr->engine_ptr->physicsEngine.getPos(id).x * 1000.0f) / 1000.0f;
        float y = round(ptr->engine_ptr->physicsEngine.getPos(id).y * 1000.0f) / 1000.0f;
        float vx = round(ptr->engine_ptr->physicsEngine.getVelocity(id).x * 1000.0f) / 1000.0f;
        float vy = round(ptr->engine_ptr->physicsEngine.getVelocity(id).y * 1000.0f) / 1000.0f;
        ImGui::Text(std::format("Body {}", id).data());
        if (ImGui::InputFloat("x##1", &x, 0.1f, 0.2f, "%.2f", 0))
        {
            ptr->engine_ptr->physicsEngine.setPos(id, glm::vec2(x, y));
        }
        if (ImGui::InputFloat("y##1", &y, 0.1f, 0.2f, "%.2f", 0))
        {
            ptr->engine_ptr->physicsEngine.setPos(id, glm::vec2(x, y));
        }
        if (ImGui::InputFloat("vx##1", &vx, 0.1f, 0.2f, "%.2f", 0))
        {
            ptr->engine_ptr->physicsEngine.setVelocity(id, glm::vec2(vx, vy));
        }
        if (ImGui::InputFloat("vy##1", &vy, 0.1, 0.2f, "%.2f", 0))
        {
            ptr->engine_ptr->physicsEngine.setVelocity(id, glm::vec2(vx, vy));
        }

        if (ImGui::Button("Next"))
        {
            id++;
            if (id > ptr->engine_ptr->physicsEngine.get_bodies().size())
            {
                id = 1;
            }
        }
    }
    else
    {
        ImGui::Text("Add bodies to get information");
    }

    ImGui::End();
}

BodyListWidget::BodyListWidget(Dispatcher* ptr, BodyInspectorWidget* inspector_ptr)
{
    this->ptr = ptr;
    this->inspector_ptr = inspector_ptr;
}

void BodyListWidget::draw()
{
    ImVec2 displaySize = ImGui::GetIO().DisplaySize;
    if (ImGui::GetItemRectSize().y < 120)
    {
        ImGui::SetNextWindowSize(ImVec2(displaySize.x, 120));
    }
    else
    {
        ImGui::SetNextWindowSize(ImVec2(displaySize.x, ImGui::GetItemRectSize().y));
    }
    ImGui::Begin("BodyList");


    if (ptr->engine_ptr->physicsEngine.get_bodies().size() > 0)
    {
        show = true;
    }


    if (show)
    {
        for (auto it : ptr->engine_ptr->physicsEngine.get_ids())
        {
            ImGui::Text(std::format("Body {}", it.first).data());
            if (!(inspector_ptr->id == it.first))
            {
                ImGui::SameLine();
                if (ImGui::Button(std::format("select##{}", it.first).data()))
                {
                    inspector_ptr->id = it.first;
                }
            }
            
        }
    }

    else
    {
        ImGui::Text("Add bodies to get information");
    }

    ImGui::End();
}


void GuiManager::init(Dispatcher* ptr)
{
    widgets.push_back(new InputWidget());
    widgets.push_back(new BodyInspectorWidget(ptr));
    widgets.push_back(new BodyListWidget(ptr, (BodyInspectorWidget*)widgets.back()));
    widgets.push_back(new WorldInspectorWidget(ptr));

}

void GuiManager::draw()
{
    for (auto& widget : widgets)
    {
        widget->draw();
    }
}

void GraphicsEngine::update(float dt)
{
    glfwMakeContextCurrent(window);
    glfwGetWindowSize(window, &window_width, &window_height);
    aspect = (float)window_width / (float)window_height;
    glViewport(0, 0, window_width, window_height);
    
    view = glm::translate(glm::ortho(-aspect, aspect, -1.0f, 1.0f), glm::vec3(-camera_pos, 0.0f)*GraphicsEngine::zoom);
    view = glm::scale(view, glm::vec3(GraphicsEngine::zoom, GraphicsEngine::zoom, 1.0f));

    glUseProgram(bodyShaderProgram);
    while (!objectQueue.empty())
    {
        add_object(create_object(objectQueue.front()));
        objectQueue.pop();

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
    updateCamera();

    glfwSwapBuffers(window);
    glfwPollEvents();
    
    glfwMakeContextCurrent(GUIwindow);

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    gui.draw();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    
    glfwSwapBuffers(GUIwindow);
    glfwPollEvents();
    glfwMakeContextCurrent(0);

    if (!running.load()) return;

    if (glfwWindowShouldClose(window) || glfwWindowShouldClose(GUIwindow)) {
        stop();
        return;
    }
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
    GraphicsEngine::zoom = zoom;
}

void GraphicsEngine::Zoom(float zoom)
{
    GraphicsEngine::zoom *= zoom;
}

void GraphicsEngine::setGrid(float gridSize, float gridThickness)
{
    this->gridSize = gridSize;
    this->gridThickness = gridThickness;
}

void GraphicsEngine::init_window(Dispatcher* ptr)
{
    gui.init(ptr);
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

    GUIwindow = glfwCreateWindow(800, 600,
        "Physics Simulation Engine GUI", NULL, NULL);

    if (!GUIwindow) {
        logger.error("Failed to create window!");
        glfwTerminate();
        return;
    }

    glfwSetScrollCallback(window, scrollCallback);

    glfwMakeContextCurrent(GUIwindow);

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);


    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO(); (void)io;

    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(GUIwindow, true);
    ImGui_ImplOpenGL3_Init("#version 330");

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
    glfwMakeContextCurrent(window);
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

void GraphicsEngine::updateCamera()
{
    double mouseX, mouseY;
    glfwGetCursorPos(window, &mouseX, &mouseY);
    float x = (2.0f * mouseX) / window_width - 1.0f;
    float y = 1.0f - (2.0f * mouseY) / window_height;
    glm::vec2 currPos(x, y);
    glm::vec4 currPos4(glm::inverse(view) * glm::vec4(x, y, 0.0f, 1.0f));
    currPos = glm::vec2(currPos4.x, currPos4.y);


    //logger.info(std::format("pos: {} {}", currPos.x, currPos.y));

    glm::vec2 delta = currPos - prevMousePos;

    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
        if (!isClicked)
        {
            clickedCoordinate = round(currPos*100.0f)/100.0f;
            isClicked = true;
            click.store(true);
        }
        if (glm::length(delta) > 0.02f)
        {
            moveCamera(-delta);
            prevMousePos = currPos - delta;
        }
        
        
    }
    else
    {
        isClicked = false;
        prevMousePos = currPos;
    }
    

    

}

void GraphicsEngine::scrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
    Zoom(std::pow(1.2f, static_cast<float>(yoffset)));
}

void GraphicsEngine::stop()
{
	glfwTerminate();
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
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


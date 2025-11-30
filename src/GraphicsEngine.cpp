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

void GraphicsEngine::update(float dt)
{
    if (!running.load()) return;

    if (glfwWindowShouldClose(window)) {
        stop();
        return;
    }

    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(shaderProgram);
    glBindVertexArray(VertexArrayID);

    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glVertexAttribPointer(
        0,
        2,
        GL_FLOAT,
        GL_FALSE,
        0,
        (void*)0
    );

    glDrawArrays(GL_TRIANGLES, 0, 3);

    glDisableVertexAttribArray(0);

    glfwSwapBuffers(window);
    glfwPollEvents();
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

    glViewport(0, 0, window_width, window_height);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

    // --- SHADERS ---
    const char* vertex_shader_src =
        "#version 330 core\n"
        "layout(location = 0) in vec2 position;\n"
        "void main() {\n"
        "    gl_Position = vec4(position, 0.0, 1.0);\n"
        "}\n";

    const char* fragment_shader_src =
        "#version 330 core\n"
        "out vec4 FragColor;\n"
        "void main() {\n"
        "    FragColor = vec4(1.0, 1.0, 1.0, 1.0);\n"
        "}\n";

    GLuint vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs, 1, &vertex_shader_src, NULL);
    glCompileShader(vs);

    GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, 1, &fragment_shader_src, NULL);
    glCompileShader(fs);

    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vs);
    glAttachShader(shaderProgram, fs);
    glLinkProgram(shaderProgram);

    glDeleteShader(vs);
    glDeleteShader(fs);

    // --- VAO/VBO ---
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);

    static const GLfloat vertices[] = {
        -1.0f, -1.0f,
         1.0f, -1.0f,
         0.0f,  1.0f
    };

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
}
void GraphicsEngine::stop()
{
	glfwTerminate();
	running.store(false);
}




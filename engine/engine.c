#include "engine.h"
#include "utils.h"
#include <stdio.h>

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

bool CompileShader(const char* shaderPath, GLenum shaderType, char infoLog[512], unsigned int* shader)
{
    char* shaderSource = ReadFile(shaderPath);
    if (!shaderSource)
        return false;

    *shader = glCreateShader(shaderType);
    glShaderSource(*shader, 1, &shaderSource, NULL);
    glCompileShader(*shader);
    free(shaderSource);

    int success;
    glGetShaderiv(*shader, GL_COMPILE_STATUS, &success);
    if(!success) {
        if (infoLog) {
            glGetShaderInfoLog(*shader, 512, NULL, infoLog);
            printf("ERROR: %d shader compilation failed, infolog: %s\n", shaderType, infoLog);
        }
        return false;
    }

    return true;
}

GLFWwindow* InitEngine(int windowWidth, int windowHeight) {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(windowWidth, windowHeight, "OGLE", NULL, NULL);
    if (!window) {
        printf("Failed to create GLFW window\n");
        glfwTerminate();
        return NULL;
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        printf("Failed to init GLAD\n");
        return NULL;
    }

    glViewport(0, 0, windowWidth, windowHeight);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    return window;
}

bool CompileShaderProgramFromFiles(const char* vertexPath, const char* fragmentPath, unsigned int* shaderProgram, char infoLog[512]) {
    unsigned int vertexShader;
    if (!CompileShader(vertexPath, GL_VERTEX_SHADER, infoLog, &vertexShader))
        return false;
    
    unsigned int fragmentShader;
    if (!CompileShader(fragmentPath, GL_FRAGMENT_SHADER, infoLog, &fragmentShader))
        return false;
    
    *shaderProgram = glCreateProgram();
    glAttachShader(*shaderProgram, vertexShader);
    glAttachShader(*shaderProgram, fragmentShader);
    glLinkProgram(*shaderProgram);
    
    int success;
    glGetProgramiv(*shaderProgram, GL_LINK_STATUS, &success);
    if(!success) {
        if (infoLog) {
            glGetProgramInfoLog(*shaderProgram, 512, NULL, infoLog);
            printf("ERROR: linking shader program failed, infolog: %s\n", infoLog);
        }
        return false;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    return true;
}
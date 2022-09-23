#include "engine.h"
#include "utils.h"
#include <stdio.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

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

    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    //glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    stbi_set_flip_vertically_on_load(true);

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

bool LoadImageAndGenTexture(const char* name, int* width, int* height, int* nrChannels, unsigned int* texture, GLenum format) {
    unsigned char* data = stbi_load(name, width, height, nrChannels, 0);
    if (!data) {
        printf("stbi_load failed to load: %s", name);
        return false;
    }

    glGenTextures(1, texture);
    glBindTexture(GL_TEXTURE_2D, *texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, *width, *height, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, 0);

    stbi_image_free(data);
    return true;
}
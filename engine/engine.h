#ifndef ENGINE_H
#define ENGINE_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdbool.h>

GLFWwindow* EngineInit(int windowWidth, int windowHeight);
bool CompileShaderProgramFromFiles(const char* vertexPath, const char* fragmentPath, unsigned int* shaderProgram, char infoLog[512]);
bool LoadImageAndGenTexture(const char* name, int* width, int* height, int* nrChannels, unsigned int* texture, GLenum format);

#endif
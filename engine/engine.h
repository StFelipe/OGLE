#ifndef ENGINE_H
#define ENGINE_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdbool.h>
#include "cglm/struct.h"

#define WORLD_UP (vec3s){0.0f, 1.0f, 0.0f}

GLFWwindow* engine_init(int windowWidth, int windowHeight);
bool shader_compileFromFiles(const char* vertexPath, const char* fragmentPath, unsigned int* shaderProgram, char infoLog[512]);
bool image_loadAndGenTexture(const char* name, int* width, int* height, int* nrChannels, unsigned int* texture, GLenum format);

// ------------- TRANSFORM -------------
typedef struct Transform {
    vec3s position;
    versors rotation;
    vec3s scale;
} Transform;

vec3s quaternion_forward(versors quaternion);
vec3s quaternion_right(versors quaternion);
vec3s quaternion_up(versors quaternion);
// ------------- TRANSFORM -------------

#endif
#ifndef ENGINE_H
#define ENGINE_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdbool.h>
#include "cglm/struct.h"

#define WORLD_UP (vec3s){0.0f, 1.0f, 0.0f}

GLFWwindow* engine_init(int windowWidth, int windowHeight);

// ------------- SHADER -------------
typedef struct {
    unsigned int id;
    char* vertexPath;
    char* fragmentPath
} Shader;

bool shader_compileFromFiles(Shader* shader, char infoLog[512]);
bool shader_recompile(Shader* shader, char infoLog[512]);
// ------------- SHADER -------------

// ------------- IMAGE & TEXTURE -------------
typedef struct {
    int width, height, nrChannels;
    unsigned int id;
} Texture;
bool texture_loadAndGen(const char* name, Texture* texture, GLenum format);
// ------------- IMAGE & TEXTURE -------------

// ------------- TRANSFORM -------------
typedef struct {
    vec3s position;
    versors rotation;
    vec3s scale;
} Transform;

vec3s quaternion_forward(versors quaternion);
vec3s quaternion_right(versors quaternion);
vec3s quaternion_up(versors quaternion);
// ------------- TRANSFORM -------------

// ------------- CAMERA -------------
typedef struct {
    vec3s position;
    vec3s forward;
    vec3s right;
    vec3s up;

    float yaw;
    float pitch;
} Camera;

Camera camera_create(vec3s position, vec3s target);
Camera camera_update(Camera camera, vec3s moveDir, float yawDelta, float pitchDelta);
// ------------- CAMERA -------------

#endif
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdbool.h>
#include <stdio.h>
#include <math.h>

#include "engine.h"
#include "cglm/struct.h"

#define WINDOW_START_WIDTH 1920
#define WINDOW_START_HEIGHT 1080

#define LOOK_SENSITIVITY 0.025
#define MOVE_SPEED_CHANGE 1

#define FRACTAL_VALS_CHANGE_SPEED 1
#define NUM_STEPS_CHANGE_SPEED 20

#define FOV 45


Shader shader;
char infoLog[512];
double moveSpeed = 5.0;

double mouseX = 0;
double mouseY = 0;
double mouseDeltaX = 0;
double mouseDeltaY = 0;
bool ignoreMouseChange = true;

unsigned int resolutionLoc;
unsigned int moveSpeedLoc;
unsigned int bailoutLoc;
unsigned int powerLoc;
unsigned int iterationsLoc;
unsigned int numberOfStepsLoc;

unsigned int cameraPosLoc;
unsigned int cameraDirLoc;
unsigned int cameraRightLoc;
unsigned int cameraUpLoc;

double bailout = 5;
double power = 8;
double iterations = 20;
double numberOfSteps = 500;

void UpdateShader(unsigned int shaderId) {
    glUseProgram(shaderId);
    resolutionLoc = glGetUniformLocation(shader.id, "resolution");
    moveSpeedLoc = glGetUniformLocation(shader.id, "moveSpeed");
    bailoutLoc = glGetUniformLocation(shader.id, "bailout");
    powerLoc = glGetUniformLocation(shader.id, "power");
    iterationsLoc = glGetUniformLocation(shader.id, "iterations");
    numberOfStepsLoc = glGetUniformLocation(shader.id, "numberOfSteps");

    cameraPosLoc = glGetUniformLocation(shader.id, "cameraPos");
    cameraDirLoc = glGetUniformLocation(shader.id, "cameraDir");
    cameraRightLoc = glGetUniformLocation(shader.id, "cameraRight");
    cameraUpLoc = glGetUniformLocation(shader.id, "cameraUp");
}

mat4s projection_create(mat4s projection, int width, int height) {
    return glms_perspective(glm_rad(FOV), (float)width / height, 0.1, 100);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    if (ignoreMouseChange || glfwGetInputMode(window, GLFW_CURSOR) == GLFW_CURSOR_NORMAL) {
        mouseX = xpos;
        mouseY = ypos;
        ignoreMouseChange = false;
    }

    mouseDeltaX = xpos - mouseX;
    mouseDeltaY = ypos - mouseY;
    mouseX = xpos;
    mouseY = ypos;
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    if (yoffset > 0)
        moveSpeed *= 1.2;
    else
        moveSpeed /= 1.2;
}


void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ENTER && action == GLFW_PRESS) {
        if (!shader_compileFromFiles(&shader, infoLog))
            return -1;

        UpdateShader(shader.id);
    }
    if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
        int mode = glfwGetInputMode(window, GLFW_CURSOR);
        ignoreMouseChange = true;
        if (mode == GLFW_CURSOR_DISABLED)
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        else
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    }
}

int main(int argc, char* argv[]) {
    int windowWidth = WINDOW_START_WIDTH;
    int windowHeight = WINDOW_START_HEIGHT;
    int viewportDimensions[4];

    mat4s view;
    mat4s projection;
    
    double deltaTime = 0;
    double lastFrame = 0;

    Camera camera = camera_create((vec3s){ 0, 0, 5 }, (vec3s){ 0.0f, 0.0f, 0.0f });

    GLFWwindow* window = engine_init(windowWidth, windowHeight);
    if (window == NULL)
        return -1;

    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetKeyCallback(window, key_callback);

    shader.vertexPath = "shaders/vertex.vs";
    shader.fragmentPath = "shaders/fragment.fs";
    if (!shader_compileFromFiles(&shader, infoLog))
        return -1;

    float vertices[] = {
        -1, 1, 0,  1, 1, 0,  1, -1, 0,
        1, -1, 0,  -1, -1, 0,  -1, 1, 0
    };

    unsigned int VBO;
    glGenBuffers(1, &VBO);

    unsigned int VAO;
    glGenVertexArrays(1, &VAO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);

    UpdateShader(shader.id);

    double timer = 0;
    while (!glfwWindowShouldClose(window))
    {
        double currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        
        timer += deltaTime;
        if (timer > 5)
        {
            timer = deltaTime;
            printf("%f\n", (1.0 / deltaTime));
        }
        
        glGetIntegerv(GL_VIEWPORT, viewportDimensions);
        windowWidth = viewportDimensions[2];
        windowHeight = viewportDimensions[3];
        projection = projection_create(projection, windowWidth, windowHeight);

        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);
        
        vec3s moveDir = GLMS_VEC3_ZERO;
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
            moveDir = glms_vec3_add(moveDir, glms_vec3_scale(camera.right, 1));
        }
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
            moveDir = glms_vec3_add(moveDir, glms_vec3_scale(camera.right, -1));
        }
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
            moveDir = glms_vec3_add(moveDir, glms_vec3_scale(camera.forward, 1));
        }
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
            moveDir = glms_vec3_add(moveDir, glms_vec3_scale(camera.forward, -1));
        }

        if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS) {
            bailout -= FRACTAL_VALS_CHANGE_SPEED * deltaTime;
        }
        if (glfwGetKey(window, GLFW_KEY_5) == GLFW_PRESS) {
            bailout += FRACTAL_VALS_CHANGE_SPEED * deltaTime;
        }

        if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
            power -= FRACTAL_VALS_CHANGE_SPEED * deltaTime;
        }
        if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS) {
            power += FRACTAL_VALS_CHANGE_SPEED * deltaTime;
        }

        if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS) {
            iterations -= FRACTAL_VALS_CHANGE_SPEED * deltaTime;
        }
        if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS) {
            iterations += FRACTAL_VALS_CHANGE_SPEED * deltaTime;
        }

        if (glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS) {
            numberOfSteps -= NUM_STEPS_CHANGE_SPEED * deltaTime;
        }
        if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS) {
            numberOfSteps += NUM_STEPS_CHANGE_SPEED * deltaTime;
        }

        moveDir = glms_vec3_scale(glms_vec3_normalize(moveDir), moveSpeed * deltaTime);
        camera = camera_update(camera, moveDir, mouseDeltaX * LOOK_SENSITIVITY, -mouseDeltaY * LOOK_SENSITIVITY);

        //view = glms_look(camera.position, camera.forward, camera.up);

        glClearColor(0, 0, 0, 1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(shader.id);

        // glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &view);
        glUniform2fv(resolutionLoc, 1, &(vec2s){windowWidth, windowHeight});
        glUniform1f(moveSpeedLoc, moveSpeed);

        glUniform1i(numberOfStepsLoc, (int)numberOfSteps);
        glUniform1i(iterationsLoc, (int)iterations);
        glUniform1f(powerLoc, power);
        glUniform1f(bailoutLoc, bailout);

        glUniform3fv(cameraPosLoc, 1, &camera.position);
        glUniform3fv(cameraDirLoc, 1, &camera.forward);
        glUniform3fv(cameraRightLoc, 1, &camera.right);
        glUniform3fv(cameraUpLoc, 1, &camera.up);

        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);

        mouseDeltaX = 0;
        mouseDeltaY = 0;
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
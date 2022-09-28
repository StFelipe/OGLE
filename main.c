#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdbool.h>
#include <stdio.h>
#include <math.h>

#include "engine.h"
#include "cglm/struct.h"

#define WINDOW_START_WIDTH 800
#define WINDOW_START_HEIGHT 600

#define MOVE_SPEED 5.0
#define LOOK_SENSITIVITY 0.05

#define FOV 45


Shader shader;
char infoLog[512];

double mouseX = 0;
double mouseY = 0;
double mouseDeltaX = 0;
double mouseDeltaY = 0;
bool firstMouse = true;

void UpdateShader(unsigned int shaderId) {
    glUseProgram(shaderId);
    glUniform1i(glGetUniformLocation(shaderId, "objectTexture"), 0);
    glUniform3fv(glGetUniformLocation(shaderId, "objectColor"), 1, &(vec3s){1, 1, 1});
    glUniform3fv(glGetUniformLocation(shaderId, "lightColor"), 1, &(vec3s){1, 1, 1});
    glUniform3fv(glGetUniformLocation(shaderId, "lightDirection"), 1, &(vec3s){0.2f, 1.0f, 0.3f});
}

mat4s projection_create(mat4s projection, int width, int height) {
    return glms_perspective(glm_rad(FOV), (float)width / height, 0.1, 100);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    if (firstMouse) {
        mouseX = xpos;
        mouseY = ypos;
        firstMouse = false;
    }

    mouseDeltaX = xpos - mouseX;
    mouseDeltaY = ypos - mouseY;
    mouseX = xpos;
    mouseY = ypos;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ENTER && action == GLFW_PRESS) {
        if (!shader_compileFromFiles(&shader, infoLog))
            return -1;

        UpdateShader(shader.id);
    }
}

int main(int argc, char* argv[]) {
    int windowWidth = 800;
    int windowHeight = 600;
    int viewportDimensions[4];

    mat4s view;
    mat4s projection;
    
    double deltaTime = 0;
    double lastFrame = 0;

    Camera camera = camera_create((vec3s){ 0, 0, 3 }, (vec3s){ 0.0f, 0.0f, 0.0f });

    GLFWwindow* window = engine_init(windowWidth, windowHeight);
    if (window == NULL)
        return -1;

    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetKeyCallback(window, key_callback);

    shader.vertexPath = "shaders/vertex.vs";
    shader.fragmentPath = "shaders/fragment.fs";
    if (!shader_compileFromFiles(&shader, infoLog))
        return -1;

    Texture texture;
    if (!texture_loadAndGen("wall.jpg", &texture, GL_RGB))
        return -1;

    float vertices[] = {
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
        0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
        0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
        0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,  0.0f, 0.0f,
        0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,  1.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,  1.0f, 1.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,  1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,  0.0f, 0.0f,

        -0.5f,  0.5f,  0.5f,  -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

        0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
        0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
        0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
        0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
        0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
        0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f
    };

    unsigned int VBO;
    glGenBuffers(1, &VBO);

    unsigned int VAO;
    glGenVertexArrays(1, &VAO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    UpdateShader(shader.id);
    glUseProgram(shader.id);
    unsigned int modelLoc = glGetUniformLocation(shader.id, "model");
    unsigned int modelInvLoc = glGetUniformLocation(shader.id, "modelInv");
    unsigned int viewLoc = glGetUniformLocation(shader.id, "view");
    unsigned int projectionLoc = glGetUniformLocation(shader.id, "projection");


    vec3s cubePositions[] = {
        (vec3s){0.0f,  0.0f,  0.0f}, 
        (vec3s){2.0f,  5.0f, -15.0f}, 
        (vec3s){-1.5f, -2.2f, -2.5f}, 
        (vec3s){-3.8f, -2.0f, -12.3f}, 
        (vec3s){-1.7f,  3.0f, -7.5f}, 
        (vec3s){1.3f, -2.0f, -2.5f}, 
        (vec3s){1.5f,  2.0f, -2.5f}, 
        (vec3s){1.5f,  0.2f, -1.5f}, 
        (vec3s){-1.3f,  1.0f, -1.5f}
    };


    double timer = 0;
    while (!glfwWindowShouldClose(window))
    {
        double currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        
        timer += deltaTime;
        if (timer > 0.5)
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

        moveDir = glms_vec3_scale(glms_vec3_normalize(moveDir), MOVE_SPEED * deltaTime);
        camera = camera_update(camera, moveDir, mouseDeltaX * LOOK_SENSITIVITY, -mouseDeltaY * LOOK_SENSITIVITY);

        view = glms_look(camera.position, camera.forward, camera.up);

        glClearColor(0.05f, 0.08f, 0.12f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(shader.id);

        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &view);
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, &projection);
        
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture.id);

        glBindVertexArray(VAO);

        for(int i = 0; i < 9; i++)
        {
            mat4s model = GLMS_MAT4_IDENTITY;
            model = glms_translate(model, cubePositions[i]);
            float angle = 20.0f * i;
            model = glms_rotate(model, glm_rad(angle), (vec3s){1, -0.6, 0.5});
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, &model);
            mat4s modelInv = glms_mat4_inv(model);
            glUniformMatrix4fv(modelInvLoc, 1, GL_FALSE, &modelInv);

            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        glBindVertexArray(0);

        mouseDeltaX = 0;
        mouseDeltaY = 0;
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
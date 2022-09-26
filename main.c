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

mat4s projection_create(mat4s projection, int width, int height) {
    return glms_perspective(glm_rad(FOV), (float)width / height, 0.1, 100);
}


double mouseX = 0;
double mouseY = 0;
double mouseDeltaX = 0;
double mouseDeltaY = 0;
bool firstMouse = true;
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

int main(int argc, char* argv[]) {
    int windowWidth = 800;
    int windowHeight = 600;
    int viewportDimensions[4];

    GLFWwindow* window = engine_init(windowWidth, windowHeight);
    if (window == NULL)
        return -1;

    glfwSetCursorPosCallback(window, mouse_callback);

    char infoLog[512];

    unsigned int shaderProgram;
    if (!shader_compileFromFiles("shaders/vertex.vs", "shaders/fragment.fs", &shaderProgram, infoLog))
        return -1;

    int width, height, nrChannels;
    unsigned int texture;
    if (!image_loadAndGenTexture("wall.jpg", &width, &height, &nrChannels, &texture, GL_RGB))
        return -1;

    int width2, height2, nrChannels2;
    unsigned int texture2;
    if (!image_loadAndGenTexture("awesomeface.png", &width2, &height2, &nrChannels2, &texture2, GL_RGBA))
        return -1;

    float vertices[] = {
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
};
    // unsigned int indices[] = {
    //     0, 1, 2,
    //     2, 3, 0
    // };
    
    // unsigned int EBO;
    // glGenBuffers(1, &EBO);

    unsigned int VBO;
    glGenBuffers(1, &VBO);

    unsigned int VAO;
    glGenVertexArrays(1, &VAO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    // glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW); 
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);

    glUseProgram(shaderProgram);
    glUniform1i(glGetUniformLocation(shaderProgram, "texture1"), 0);
    glUniform1i(glGetUniformLocation(shaderProgram, "texture2"), 1);

    vec3s cameraPos = {0, 0, 3};
    vec3s cameraTarget = {0.0f, 0.0f, 0.0f};
    vec3s cameraForward = glms_vec3_normalize(glms_vec3_sub(cameraTarget, cameraPos));
    vec3s cameraRight = glms_vec3_normalize(glms_vec3_cross(cameraForward, WORLD_UP));
    vec3s cameraUp = glms_vec3_normalize(glms_vec3_cross(cameraRight, cameraForward));

    float cameraYaw = -90;
    float cameraPitch = 0;

    mat4s view;
    mat4s projection;


    unsigned int modelLoc = glGetUniformLocation(shaderProgram, "model");
    unsigned int viewLoc = glGetUniformLocation(shaderProgram, "view");
    unsigned int projectionLoc = glGetUniformLocation(shaderProgram, "projection");


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
    
    double deltaTime = 0;
    double lastFrame = 0;

    float moveSpeed;
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
            moveDir = glms_vec3_add(moveDir, glms_vec3_scale(cameraRight, 1));
        }
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
            moveDir = glms_vec3_add(moveDir, glms_vec3_scale(cameraRight, -1));
        }
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
            moveDir = glms_vec3_add(moveDir, glms_vec3_scale(cameraForward, 1));
        }
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
            moveDir = glms_vec3_add(moveDir, glms_vec3_scale(cameraForward, -1));
        }
        cameraPos = glms_vec3_add(cameraPos, glms_vec3_scale(glms_vec3_normalize(moveDir), MOVE_SPEED * deltaTime));

        cameraYaw += mouseDeltaX * LOOK_SENSITIVITY;
        cameraPitch += -mouseDeltaY * LOOK_SENSITIVITY;

        if(cameraPitch > 89.0f)
            cameraPitch = 89.0f;
        if(cameraPitch < -89.0f)
            cameraPitch = -89.0f;

        vec3s direction;
        direction.x = cos(glm_rad(cameraYaw)) * cos(glm_rad(cameraPitch));
        direction.y = sin(glm_rad(cameraPitch));
        direction.z = sin(glm_rad(cameraYaw)) * cos(glm_rad(cameraPitch));
        cameraForward = glms_vec3_normalize(direction);
        cameraRight = glms_vec3_normalize(glms_vec3_cross(cameraForward, WORLD_UP));
        cameraUp = glms_vec3_normalize(glms_vec3_cross(cameraRight, cameraForward));

        // versors pitch = glms_quat_normalize(glms_quatv(-mouseDeltaY * LOOK_SENSITIVITY, quaternion_right(cameraTrans.rotation)));
        // versors yaw = glms_quat_normalize(glms_quat(mouseDeltaX * LOOK_SENSITIVITY, 0, -1, 0));
        // cameraTrans.rotation = glms_quat_normalize(glms_quat_mul(pitch, glms_quat_normalize(glms_quat_mul(cameraTrans.rotation, yaw))));

        view = glms_look(cameraPos, cameraForward, cameraUp);

        glClearColor(0.05f, 0.08f, 0.12f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(shaderProgram);

        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &view);
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, &projection);
        
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture2);

        glBindVertexArray(VAO);
        //glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        //glDrawArrays( GL_TRIANGLES, 0, 36);

        for(int i = 0; i < 10; i++)
        {
            mat4s model = GLMS_MAT4_IDENTITY;
            model = glms_translate(model, cubePositions[i]);
            float angle = 20.0f * i;
            model = glms_rotate(model, glm_rad(angle), (vec3s){1, -0.6, 0.5});
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, &model);

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
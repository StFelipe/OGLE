#include "engine.h"
#include "utils.h"
#include <stdio.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

bool shader_compile(const char* shaderPath, GLenum shaderType, char infoLog[512], unsigned int* shader) {
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

GLFWwindow* engine_init(int windowWidth, int windowHeight) {
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

    glEnable(GL_DEPTH_TEST);
    //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    //glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    stbi_set_flip_vertically_on_load(true);

    return window;
}

bool shader_compileFromFiles(Shader* shader, char infoLog[512]) {
    unsigned int vertexShader;
    if (!shader_compile(shader->vertexPath, GL_VERTEX_SHADER, infoLog, &vertexShader))
        return false;
    
    unsigned int fragmentShader;
    if (!shader_compile(shader->fragmentPath, GL_FRAGMENT_SHADER, infoLog, &fragmentShader))
        return false;
    
    shader->id = glCreateProgram();
    glAttachShader(shader->id, vertexShader);
    glAttachShader(shader->id, fragmentShader);
    glLinkProgram(shader->id);
    
    int success;
    glGetProgramiv(shader->id, GL_LINK_STATUS, &success);
    if(!success) {
        if (infoLog) {
            glGetProgramInfoLog(shader->id, 512, NULL, infoLog);
            printf("ERROR: linking shader program failed, infolog: %s\n", infoLog);
        }
        return false;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    return true;
}

bool shader_recompile(Shader* shader, char infoLog[512]) {
    glDeleteProgram(shader->id);
    return shader_compileFromFiles(shader, infoLog);
}

bool texture_loadAndGen(const char* name, Texture* texture, GLenum format) {
    unsigned char* data = stbi_load(name, &texture->width, &texture->height, &texture->nrChannels, 0);
    if (!data) {
        printf("stbi_load failed to load: %s", name);
        return false;
    }

    glGenTextures(1, &texture->id);
    glBindTexture(GL_TEXTURE_2D, texture->id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texture->width, texture->height, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, 0);

    stbi_image_free(data);
    return true;
}



vec3s quaternion_forward(versors quaternion) {
    return glms_vec3_normalize(glms_quat_rotatev(quaternion, (vec3s){0.0f, 0.0f, -1.0f}));
}

vec3s quaternion_right(versors quaternion) {
    return glms_vec3_normalize(glms_quat_rotatev(quaternion, (vec3s){1.0f, 0.0f, 0.0f}));
}

vec3s quaternion_up(versors quaternion) {
    return glms_vec3_normalize(glms_quat_rotatev(quaternion, (vec3s){0.0f, 1.0f, 0.0f}));
}


Camera camera_create(vec3s position, vec3s target) {
    Camera camera;
    camera.position = position;
    camera.forward = glms_vec3_normalize(glms_vec3_sub(target, position));
    camera.right = glms_vec3_normalize(glms_vec3_cross(camera.forward, WORLD_UP));
    camera.up = glms_vec3_normalize(glms_vec3_cross(camera.right, camera.forward));

    // calculate somehow
    camera.yaw = -90;
    camera.pitch = 0;

    return camera;
}

Camera camera_update(Camera camera, vec3s moveDir, float yawDelta, float pitchDelta) {
    camera.position = glms_vec3_add(camera.position, moveDir);

    camera.yaw += yawDelta;
    camera.pitch += pitchDelta;

    if(camera.pitch > 89.0f)
        camera.pitch = 89.0f;
    if(camera.pitch < -89.0f)
        camera.pitch = -89.0f;

    vec3s direction;
    direction.x = cos(glm_rad(camera.yaw)) * cos(glm_rad(camera.pitch));
    direction.y = sin(glm_rad(camera.pitch));
    direction.z = sin(glm_rad(camera.yaw)) * cos(glm_rad(camera.pitch));
    camera.forward = glms_vec3_normalize(direction);
    camera.right = glms_vec3_normalize(glms_vec3_cross(camera.forward, WORLD_UP));
    camera.up = glms_vec3_normalize(glms_vec3_cross(camera.right, camera.forward));

    return camera;
}
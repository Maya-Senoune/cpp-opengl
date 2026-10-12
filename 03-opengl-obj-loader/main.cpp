#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

#include "objectLoader.h"

//vertex shader
const char* vertexShaderSource = R"glsl(
    #version 330 core
    layout (location = 0) in vec3 aPos;
    layout (location = 1) in vec3 aNormal;

    uniform mat4 transform;

    void main(){
        gl_Position = transform * vec4(aPos, 1.0);
    }
)glsl";

//fragment shader for coloring
const char* fragmentShaderSource = R"glsl(
    #version 330 core
    out vec4 FragColor;
    uniform vec4 setColor;

    void main(){
        FragColor = setColor;
    }
)glsl";

glm::mat4 transform = glm::mat4(1.0f);
//matrix for view & projection
glm::mat4 view = glm::mat4(1.0f);
glm::mat4 project = glm::mat4(1.0f);

int main()
{
    // initialize glfw
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW\n";
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // window
    GLFWwindow* window = glfwCreateWindow(1024, 768, "Assignment 3", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create window\n";
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // initialize glew
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW\n";
        return -1;
    }

    //vertex shader
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
    glCompileShader(vertexShader);

    int vertexSuccess;
    char vertexInfoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &vertexSuccess);
    if (!vertexSuccess) {
        glGetShaderInfoLog(vertexShader, 512, nullptr, vertexInfoLog);
        std::cerr << "Vertex shader failed to compile:\n" << vertexInfoLog << std::endl;
    }

    // fragment shader
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
    glCompileShader(fragmentShader);

    int fragmentSuccess;
    char fragmentInfoLog[512];
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &fragmentSuccess);
    if (!fragmentSuccess) {
        glGetShaderInfoLog(fragmentShader, 512, nullptr, fragmentInfoLog);
        std::cerr << "Fragment shader failed to compile:\n" << fragmentInfoLog << std::endl;
    }

    // link shaders
    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    int linkSuccess;
    char linkInfoLog[512];
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &linkSuccess);
    if (!linkSuccess) {
        glGetProgramInfoLog(shaderProgram, 512, nullptr, linkInfoLog);
        std::cerr << "Shader program failed to link:\n" << linkInfoLog << std::endl;
    }

    //load the chair (see objectLoader.cpp) & draw as wireframe
    Mesh chair = LoadOBJ("chair.obj");
    chair.uploadToGPU();
    glEnable(GL_DEPTH_TEST);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    //setup camera so the whole chair fits on screen
    glm::vec3 extent = chair.boundingBoxMax - chair.boundingBoxMin;
    float radius = glm::length(extent) * 0.5f;
    glm::vec3 target = chair.center();
    glm::vec3 eye = target + glm::vec3(0.0f, radius * 0.3f, radius * 2.5f);

    view = glm::lookAt(eye, target, glm::vec3(0.0f, 1.0f, 0.0f));
    project = glm::perspective(glm::radians(45.0f), 1024.0f / 768.0f, 0.1f, 1000.0f);

    GLuint transformLoc = glGetUniformLocation(shaderProgram, "transform");
    GLuint colorLoc = glGetUniformLocation(shaderProgram, "setColor");

    // render loop
    while (!glfwWindowShouldClose(window)) {
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(shaderProgram);

        //combining the matrices
        glm::mat4 finalMatrix = project * view * transform;
        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(finalMatrix));
        glUniform4f(colorLoc, 1.0f, 1.0f, 1.0f, 1.0f);

        chair.draw();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
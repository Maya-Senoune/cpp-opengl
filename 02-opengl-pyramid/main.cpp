#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

//additional constants/functions for shaders
const char* vertexShaderSource = R"glsl(
#version 330 core
layout (location = 0) in vec3 aPos;

uniform mat4 transform;

void main()
{
    gl_Position = transform * vec4(aPos, 1.0);
}
)glsl";

//fragment shader for coloring 
const char* fragmentShaderSource = R"glsl(
#version 330 core
out vec4 FragColor;

void main()
{
    FragColor = vec4(0.68, 0.85, 0.90, 1.0);
}
)glsl";


// data of pyramid where xyz (5 vertices in the space)
float pyramidVertices[] = {
    -0.5f, -0.5f, -0.5f,
     0.5f, -0.5f, -0.5f,
     0.5f, -0.5f,  0.5f,
    -0.5f, -0.5f,  0.5f,
     0.0f,  0.5f,  0.0f
};

//pyramid built off of triangles (one for each wall + 2 for the bottom cuz its a square)
unsigned int pyramidIndices[] = {
    0, 1, 2,
    0, 2, 3,
    0, 1, 4,
    1, 2, 4,
    2, 3, 4,
    3, 0, 4
};

unsigned int VBO[2], VAO[2], EBO;

//matrix for the transformation
glm::mat4 transform = glm::mat4(1.0f);
const float d = 0.1f;  //hardcoded d value
const float s = 1.1f; //hardcoded s value

void setupPyramid()
{
    //containers to store shape
    glGenVertexArrays(2, VAO);
    glGenBuffers(2, VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO[0]);

    glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(pyramidVertices), pyramidVertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(pyramidIndices), pyramidIndices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
}
//keycallback function
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (action != GLFW_PRESS && action != GLFW_REPEAT)
        return;

    switch (key)
    {
    case GLFW_KEY_W: transform = glm::translate(transform, glm::vec3(0.0f, d, 0.0f)); break; //up
    case GLFW_KEY_S: transform = glm::translate(transform, glm::vec3(0.0f, -d, 0.0f)); break; //down
    case GLFW_KEY_A: transform = glm::translate(transform, glm::vec3(-d, 0.0f, 0.0f)); break;//left
    case GLFW_KEY_D: transform = glm::translate(transform, glm::vec3(d, 0.0f, 0.0f)); break;//right

    case GLFW_KEY_Q: transform = glm::rotate(transform, glm::radians(30.0f), glm::vec3(0.0f, 0.0f, 1.0f)); break;// z direction anticlockwise
    case GLFW_KEY_E: transform = glm::rotate(transform, glm::radians(-30.0f), glm::vec3(0.0f, 0.0f, 1.0f)); break;//z direcitonclockwise

    case GLFW_KEY_R: transform = glm::scale(transform, glm::vec3(1.0f, 1.0f, s)); break;//scale in +z direction
    case GLFW_KEY_F: transform = glm::scale(transform, glm::vec3(1.0f, 1.0f, -s)); break;//scalein -z direction
    }
}

int main()
{
    // inisitalize glfw
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW\n";
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // window
    GLFWwindow* window = glfwCreateWindow(800, 600, "OpenGL Test", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create window\n";
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, keyCallback);//set func to call te he keycallback function

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
    if (!vertexSuccess)
    {
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
    if (!fragmentSuccess)
    {
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
    if (!linkSuccess)
    {
        glGetProgramInfoLog(shaderProgram, 512, nullptr, linkInfoLog);
        std::cerr << "Shader program failed to link:\n" << linkInfoLog << std::endl;
    }

    //setup pyramid
    setupPyramid();
    glEnable(GL_DEPTH_TEST);

    GLuint transformLoc = glGetUniformLocation(shaderProgram, "transform");

    // render loop
    while (!glfwWindowShouldClose(window))
    {
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(shaderProgram);

        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));

        glBindVertexArray(VAO[0]);
        glDrawElements(GL_TRIANGLES, 18, GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
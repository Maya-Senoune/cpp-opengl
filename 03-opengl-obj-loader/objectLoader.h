#pragma once
#include <string>
#include <vector>
#include <GL/glew.h>
#include <glm/glm.hpp>

struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
};

struct Mesh {
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    glm::vec3 boundingBoxMin{ 0.0f };
    glm::vec3 boundingBoxMax{ 0.0f };
    glm::vec3 center() const { return (boundingBoxMin + boundingBoxMax) * 0.5f; }

    GLuint VAO = 0, VBO = 0, EBO = 0;

    void uploadToGPU();
    void draw() const;
};

Mesh LoadOBJ(const std::string& path);

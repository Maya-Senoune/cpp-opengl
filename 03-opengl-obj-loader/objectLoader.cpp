#include "objectLoader.h"
#include <fstream>
#include <iostream>
#include <limits>

// reads chair.obj & builds mesh out of it
Mesh LoadOBJ(const std::string& path) {
    std::ifstream file(path);
    std::vector<glm::vec3> positions{ glm::vec3(0.0f) };
    std::vector<glm::vec3> normals{ glm::vec3(0.0f) };

    Mesh mesh;
    //bounding box
    mesh.boundingBoxMin = glm::vec3(std::numeric_limits<float>::max());
    mesh.boundingBoxMax = glm::vec3(std::numeric_limits<float>::lowest());

    std::string tag;
    while (file >> tag) {

        if (tag == "v") {
            // vertex position v x y z
            glm::vec3 p;
            file >> p.x >> p.y >> p.z;
            positions.push_back(p);

            mesh.boundingBoxMin = glm::min(mesh.boundingBoxMin, p);
            mesh.boundingBoxMax = glm::max(mesh.boundingBoxMax, p);
        }
        else if (tag == "vn") {
            // normal (direction) vn x y z
            glm::vec3 n;
            file >> n.x >> n.y >> n.z;
            normals.push_back(n);
        }
        else if (tag == "f") {

            std::vector<Vertex> faceVerts;
            for (int corner = 0; corner < 4; corner++) {
                int v, vt, vn;
                char slash;
                file >> v >> slash >> vt >> slash >> vn;
                Vertex vert;
                vert.position = positions[v];
                vert.normal = normals[vn];
                faceVerts.push_back(vert);
            }

            // fix to triangles for gpu
            for (size_t i = 1; i + 1 < faceVerts.size(); i++) {
                mesh.vertices.push_back(faceVerts[0]);
                mesh.vertices.push_back(faceVerts[i]);
                mesh.vertices.push_back(faceVerts[i + 1]);
            }
        }
    }
    std::cout << "LoadOBJ: " << path << " -> " << mesh.vertices.size() << " vertices\n";
    return mesh;
}

// copies the finished vertex list to GPU
void Mesh::uploadToGPU() {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}

// tell gpu to draw everything in vertex buffer,
void Mesh::draw() const {
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, (GLsizei)vertices.size());
    glBindVertexArray(0);
}
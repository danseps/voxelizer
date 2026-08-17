/**
 * @file mesh.hpp
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2026-08-16
 * 
 * @copyright Copyright (c) 2026
 * 
 */
#pragma once
#include <glad/glad.h>
#include "chunkMesher.hpp"

class Mesh
{
private:
    GLuint VAO, VBO, EBO;
    GLsizei indexCount;
public:
    Mesh() : VAO(0), VBO(0), EBO(0), indexCount(0) {};

    ~Mesh() 
    {
        clear();
    }

    /**
     * @brief Clears the mesh data and deletes GPU buffers
     * 
     */
    void clear()
    {
        if (VAO != 0)
        {
            glDeleteVertexArrays(1, &VAO);
            glDeleteBuffers(1, &VBO);
            glDeleteBuffers(1, &EBO);
            VAO = 0;
            VBO = 0;
            EBO = 0;
            indexCount = 0;
        }
    }

    /**
     * @brief Uploads mesh data to GPU buffers and sets up vertex attributes
     * 
     * @param meshData 
     */
    void uploadData(const ChunkMesher::MeshData& meshData)
    {
        clear(); // Clear existing buffers if any

        if (meshData.vertices.empty() || meshData.indices.empty()) {
            return;
        }

        indexCount = static_cast<GLsizei>(meshData.indices.size());

        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
        glBindVertexArray(VAO);

        // vertex buffer
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, 
            sizeof(ChunkMesher::Vertex) * meshData.vertices.size(), 
            meshData.vertices.data(), 
            GL_STATIC_DRAW
        );

        // index buffer
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, 
            sizeof(uint32_t) * meshData.indices.size(), 
            meshData.indices.data(), 
            GL_STATIC_DRAW
        );

        size_t stride = sizeof(ChunkMesher::Vertex);
        // position attributes
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)offsetof(ChunkMesher::Vertex, x));
        glEnableVertexAttribArray(0);
        // color attributes
        glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, stride, (void*)offsetof(ChunkMesher::Vertex, color));
        glEnableVertexAttribArray(1);
        // normal attributes //TODO: add in generated mesh
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride, (void*)offsetof(ChunkMesher::Vertex, nx));
        glEnableVertexAttribArray(2);
        // texture coordinates attributes //TODO: add in generated mesh
        glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, stride, (void*)offsetof(ChunkMesher::Vertex, u));
        glEnableVertexAttribArray(3);
        // ambient occlusion attributes
        glVertexAttribPointer(4, 1, GL_FLOAT, GL_FALSE, stride, (void*)offsetof(ChunkMesher::Vertex, ao));
        glEnableVertexAttribArray(4);

        glBindVertexArray(0); // Unbind the VAO (bind when rendering)
    }

    /**
     * @brief Draws the mesh
     * 
     */
    void draw()
    {
        if (indexCount == 0 || VAO == 0) return; // Nothing to draw 

        glBindVertexArray(VAO);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // Set polygon mode
        glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }

};
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
    void uploadData(const ChunkMesher::MeshData& meshData);
    void draw();
    void clear();

    ~Mesh() 
    {
        clear();
    }

    void uploadData(const ChunkMesher::MeshData& meshData)
    {
        clear(); // Clear existing buffers if any

        if (meshData.vertices.empty() || meshData.indices.empty()) {
            return;
        }
        // TODO: Implement the function to upload mesh data to GPU buffers

        indexCount = static_cast<GLsizei>(meshData.indices.size());
    }
};
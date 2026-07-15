/**
 * @file chunkMesher.hpp
 * @author Daniel Šeps (daniel.seps@proton.me)
 * @brief Mesh generation for chunks
 * @version 0.1
 * @date 2026-07-07
 * 
 * @copyright Copyright (c) 2026
 * 
 */
 #pragma once

 #include "chunk.hpp"
 #include "utils.hpp"
 #include <vector>
 
 namespace ChunkMesher 
 {

    /**
     * @brief 
     * 
     */
    struct Vertex
    {
        float x, y, z; // Position
        utils::Color color; // Color
    };

    /**
     * @brief 
     * 
     */
    struct MeshData
    {
        std::vector<Vertex> vertices; // Vertex data
        std::vector<uint32_t> indices; // Index data
    };
     
     /**
      * @brief Push a vertex with position and color into the mesh vector
      * 
      * @param mesh 
      * @param x 
      * @param y 
      * @param z 
      * @param color
      */
    inline void addFace(MeshData& meshData, 
                        float x0, float y0, float z0,
                        float x1, float y1, float z1,
                        float x2, float y2, float z2,
                        float x3, float y3, float z3,
                        const utils::Color& color) 
    {
            uint32_t offset = static_cast<uint32_t>(meshData.vertices.size());

            meshData.vertices.push_back({x0, y0, z0, color});
            meshData.vertices.push_back({x1, y1, z1, color});
            meshData.vertices.push_back({x2, y2, z2, color});
            meshData.vertices.push_back({x3, y3, z3, color});

            // First triangle
            meshData.indices.push_back(offset + 0);
            meshData.indices.push_back(offset + 1);
            meshData.indices.push_back(offset + 2);
            // Second triangle
            meshData.indices.push_back(offset + 2);
            meshData.indices.push_back(offset + 3);
            meshData.indices.push_back(offset + 0);
        }
        
    /**
     * @brief Get the color associated with a specific block type
     * 
     * @param blockType 
     * @return utils::Color 
    */
    inline utils::Color getBlockTypeColor(BlockType blockType)
    {
        switch (blockType)
        {//TODO: Add more colors for different block types
            case BlockType::Grass: return utils::GREEN;
            case BlockType::Dirt: return utils::BROWN;
            case BlockType::Stone: return utils::GRAY;
            case BlockType::Water: return utils::BLUE;
            case BlockType::Sand: return utils::YELLOW;
            case BlockType::Wood: return utils::BROWN;
            case BlockType::Leaves: return utils::GREEN;
            case BlockType::Snow: return utils::WHITE;
            case BlockType::Lava: return utils::RED;
            default: return utils::BLACK; // Air or unknown block type
        }
    }

        /**
         * @brief Generate a mesh for the given chunk
         * 
         * @param chunk 
         * @return MeshData 
         */
        inline MeshData generateMesh(Chunk& chunk)
        {
        MeshData meshData;
        meshData.vertices.reserve(4000); // TODO: optimalize: Reserve space for the maximum possible number of vertices
        meshData.indices.reserve(6000); // TODO: optimalize: Reserve space for the maximum possible number of indices


        for (int y = 0; y < Chunk::SIZE_Y; y++)
        {
            for (int z = 0; z < Chunk::SIZE_Z; z++)
            {
                for (int x = 0; x < Chunk::SIZE_X; x++)
                {
                    BlockType block = getBlock(chunk, x, y, z);
                    if (block == BlockType::Air)
                    {
                        continue;
                    }

                    float px = x + 1.0f;
                    float py = y + 1.0f;
                    float pz = z + 1.0f;

                    utils::Color color = getBlockTypeColor(block);

                    // 1. PŘEDNÍ STĚNA (+Z)
                    if (getBlock(chunk, x, y, z + 1) == BlockType::Air)
                        addFace(meshData, x,  y,  pz, px, y,  pz, px, py, pz, x,  py, pz, color);

                    // 2. ZADNÍ STĚNA (-Z)
                    if (getBlock(chunk, x, y, z - 1) == BlockType::Air)
                        addFace(meshData, px, y,  z, x,  y,  z, x,  py, z, px, py, z, color);

                    // 3. LEVÁ STĚNA (-X)
                    if (getBlock(chunk, x - 1, y, z) == BlockType::Air)
                        addFace(meshData, x, y,  z, x, y, pz, x, py, pz, x, py, z, color);

                    // 4. PRAVÁ STĚNA (+X)
                    if (getBlock(chunk, x + 1, y, z) == BlockType::Air)
                        addFace(meshData, px, y,  pz, px, y,  z, px, py, z, px, py, pz, color);

                    // 5. HORNÍ STĚNA (+Y)
                    if (getBlock(chunk, x, y + 1, z) == BlockType::Air)
                        addFace(meshData, x,  py, pz, px, py, pz, px, py, z, x,  py, z, color);

                    // 6. SPODNÍ STĚNA (-Y)
                    if (getBlock(chunk, x, y - 1, z) == BlockType::Air)
                        addFace(meshData, x,  y, z, px, y, z, px, y, pz, x,  y, pz, color);
                }
            }
        }
        return meshData;
    }


 }
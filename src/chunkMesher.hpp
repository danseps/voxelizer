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
     */
    inline void generateMesh(const Chunk& chunk, int chunkX, int chunkZ, MeshData& meshData)
    {
        // Vypočítáme o kolik bloků se tento chunk posune ve světě OpenGL
        float offsetX = chunkX * Chunk::SIZE_X;
        float offsetZ = chunkZ * Chunk::SIZE_Z;

        // Tady už nerezervujeme paměť, protože to se dělá až venku v Mainu
        
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

                    // APLIKACE OFFSETU: Tady řekneme OpenGL, ať tuto kostku vykreslí posunutou!
                    float wx = x + offsetX;
                    float wy = y; 
                    float wz = z + offsetZ;

                    float wpx = wx + 1.0f;
                    float wpy = wy + 1.0f;
                    float wpz = wz + 1.0f;

                    utils::Color color = getBlockTypeColor(block);

                    // A teď už používáme ty posunuté (World) souřadnice (wx, wy, wz)!
                    // 1. PŘEDNÍ STĚNA (+Z)
                    if (getBlock(chunk, x, y, z + 1) == BlockType::Air)
                        addFace(meshData, wx, wy, wpz, wpx, wy, wpz, wpx, wpy, wpz, wx, wpy, wpz, color);

                    // 2. ZADNÍ STĚNA (-Z)
                    if (getBlock(chunk, x, y, z - 1) == BlockType::Air)
                        addFace(meshData, wpx, wy, wz, wx, wy, wz, wx, wpy, wz, wpx, wpy, wz, color);

                    // 3. LEVÁ STĚNA (-X)
                    if (getBlock(chunk, x - 1, y, z) == BlockType::Air)
                        addFace(meshData, wx, wy, wz, wx, wy, wpz, wx, wpy, wpz, wx, wpy, wz, color);

                    // 4. PRAVÁ STĚNA (+X)
                    if (getBlock(chunk, x + 1, y, z) == BlockType::Air)
                        addFace(meshData, wpx, wy, wpz, wpx, wy, wz, wpx, wpy, wz, wpx, wpy, wpz, color);

                    // 5. HORNÍ STĚNA (+Y)
                    if (getBlock(chunk, x, y + 1, z) == BlockType::Air)
                        addFace(meshData, wx, wpy, wpz, wpx, wpy, wpz, wpx, wpy, wz, wx, wpy, wz, color);

                    // 6. SPODNÍ STĚNA (-Y)
                    if (getBlock(chunk, x, y - 1, z) == BlockType::Air)
                        addFace(meshData, wx, wy, wz, wpx, wy, wz, wpx, wpy, wz, wx, wy, wpz, color);
                }
            }
        }
    }


 }
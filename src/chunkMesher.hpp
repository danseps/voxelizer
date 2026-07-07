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
      * @brief Push a vertex with position and color into the mesh vector
      * 
      * @param mesh 
      * @param x 
      * @param y 
      * @param z 
      * @param color
      */
    inline void pushVertex(std::vector<float>& mesh, float x, float y, float z, const utils::Color& color) {
         mesh.push_back(x); mesh.push_back(y); mesh.push_back(z);
         mesh.push_back(color.r); mesh.push_back(color.g); mesh.push_back(color.b);
         mesh.push_back(color.a);
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
         * @return std::vector<float> 
         */
        inline std::vector<float> generateMesh(Chunk& chunk)
        {
        std::vector<float> vertices;
        vertices.reserve(10000); // TODO: optimalize: Reserve space for the maximum possible number of vertices

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
                    pushVertex(vertices, x,  y,  pz, color);
                    pushVertex(vertices, px, y,  pz, color);
                    pushVertex(vertices, px, py, pz, color);
                    pushVertex(vertices, px, py, pz, color);
                    pushVertex(vertices, x,  py, pz, color);
                    pushVertex(vertices, x,  y,  pz, color);

                    // 2. ZADNÍ STĚNA (-Z)
                    pushVertex(vertices, px, y,  z, color);
                    pushVertex(vertices, x,  y,  z, color);
                    pushVertex(vertices, x,  py, z, color);
                    pushVertex(vertices, x,  py, z, color);
                    pushVertex(vertices, px, py, z, color);
                    pushVertex(vertices, px, y,  z, color);

                    // 3. LEVÁ STĚNA (-X)
                    pushVertex(vertices, x, y,  z,  color);
                    pushVertex(vertices, x, y,  pz, color);
                    pushVertex(vertices, x, py, pz, color);
                    pushVertex(vertices, x, py, pz, color);
                    pushVertex(vertices, x, py, z,  color);
                    pushVertex(vertices, x, y,  z,  color);

                    // 4. PRAVÁ STĚNA (+X)
                    pushVertex(vertices, px, y,  pz, color);
                    pushVertex(vertices, px, y,  z,  color);
                    pushVertex(vertices, px, py, z,  color);
                    pushVertex(vertices, px, py, z,  color);
                    pushVertex(vertices, px, py, pz, color);
                    pushVertex(vertices, px, y,  pz, color);

                    // 5. HORNÍ STĚNA (+Y)
                    pushVertex(vertices, x,  py, pz, color);
                    pushVertex(vertices, px, py, pz, color);
                    pushVertex(vertices, px, py, z,  color);
                    pushVertex(vertices, px, py, z,  color);
                    pushVertex(vertices, x,  py, z,  color);
                    pushVertex(vertices, x,  py, pz, color);

                    // 6. SPODNÍ STĚNA (-Y)
                    pushVertex(vertices, x,  y, z,  color);
                    pushVertex(vertices, px, y, z,  color);
                    pushVertex(vertices, px, y, pz, color);
                    pushVertex(vertices, px, y, pz, color);
                    pushVertex(vertices, x,  y, pz, color);
                    pushVertex(vertices, x,  y, z,  color);
                }
            }
        }
        return vertices;
    }


 }
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

    enum class Face
    {
        Front,
        Back,
        Left,
        Right,
        Top,
        Bottom
    };

    /**
     * @brief 
     * 
     */
    struct Vertex
    {
        float x, y, z; // Position
        utils::Color color; // Color
        float nx, ny, nz; // Normal vector
        float u, v; // Texture coordinates
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
                        utils::Color color,
                        float nx, float ny, float nz,
                        std::pair<float, float> textureCoords) 
    {
            uint32_t offset = static_cast<uint32_t>(meshData.vertices.size());

            // textureCoords contains the tile origin in normalized [0,1] space (e.g. col/16, row/16)
            const float tileSize = 1.0f / 16.0f; // assuming a 16x16 tile atlas
            float u0 = textureCoords.first;
            float v0 = textureCoords.second;
            float u1 = u0 + tileSize;
            float v1 = v0 + tileSize;

            meshData.vertices.push_back({x0, y0, z0, color, nx, ny, nz, u0, v0});
            meshData.vertices.push_back({x1, y1, z1, color, nx, ny, nz, u1, v0});
            meshData.vertices.push_back({x2, y2, z2, color, nx, ny, nz, u1, v1});
            meshData.vertices.push_back({x3, y3, z3, color, nx, ny, nz, u0, v1});

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
     * @return std::pair<float, float> A pair representing the u and v texture coordinates for the block type
    */
    inline std::pair<float, float> tileUV(int x, int y)
    {
        return std::make_pair(x / 16.0f, y / 16.0f);
    }

    inline std::pair<float, float> getBlockTextureForFace(BlockType blockType, Face face)
    {
        switch (blockType)
        {
            case BlockType::Stone:
                return tileUV(1, 15);
            case BlockType::Dirt:
                return tileUV(2, 15);
            case BlockType::Grass:
                // Minecraft-like mapping in this atlas:
                // top = grass top, sides = grass side, bottom = dirt.
                if (face == Face::Top)
                    return tileUV(0, 15);
                if (face == Face::Bottom)
                    return tileUV(2, 15);
                return tileUV(3, 15);
            default:
                return tileUV(0, 0);
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

                    // APLIKACE OFFSETU
                    float wx = x + offsetX;
                    float wy = y; 
                    float wz = z + offsetZ;

                    float wpx = wx + 1.0f;
                    float wpy = wy + 1.0f;
                    float wpz = wz + 1.0f;

                    // A teď už používáme ty posunuté (World) souřadnice (wx, wy, wz)!
                    // 1. PŘEDNÍ STĚNA (+Z)
                    if (getBlock(chunk, x, y, z + 1) == BlockType::Air) //TODO: vytvorit a poslat cely vertex?? misto tolika cisel 
                        addFace(meshData, wx, wy, wpz, wpx, wy, wpz, wpx, wpy, wpz, wx, wpy, wpz, utils::WHITE /*TODO: pozdeji budem pouzivat u lightingu ig*/, 0.0f, 0.0f, 1.0f, getBlockTextureForFace(block, Face::Front)); // Normálový vektor pro přední stěnu je (0, 0, 1)

                    // 2. ZADNÍ STĚNA (-Z)
                    if (getBlock(chunk, x, y, z - 1) == BlockType::Air)
                        addFace(meshData, wpx, wy, wz, wx, wy, wz, wx, wpy, wz, wpx, wpy, wz, utils::WHITE, 0.0f, 0.0f, -1.0f, getBlockTextureForFace(block, Face::Back));

                    // 3. LEVÁ STĚNA (-X)
                    if (getBlock(chunk, x - 1, y, z) == BlockType::Air)
                        addFace(meshData, wx, wy, wz, wx, wy, wpz, wx, wpy, wpz, wx, wpy, wz, utils::WHITE, -1.0f, 0.0f, 0.0f, getBlockTextureForFace(block, Face::Left));

                    // 4. PRAVÁ STĚNA (+X)
                    if (getBlock(chunk, x + 1, y, z) == BlockType::Air)
                        addFace(meshData, wpx, wy, wpz, wpx, wy, wz, wpx, wpy, wz, wpx, wpy, wpz, utils::WHITE, 1.0f, 0.0f, 0.0f, getBlockTextureForFace(block, Face::Right));

                    // 5. HORNÍ STĚNA (+Y)
                    if (getBlock(chunk, x, y + 1, z) == BlockType::Air)
                        addFace(meshData, wx, wpy, wpz, wpx, wpy, wpz, wpx, wpy, wz, wx, wpy, wz, utils::WHITE, 0.0f, 1.0f, 0.0f, getBlockTextureForFace(block, Face::Top));

                    // 6. SPODNÍ STĚNA (-Y)
                    if (getBlock(chunk, x, y - 1, z) == BlockType::Air)
                        addFace(meshData, wx, wy, wz, wpx, wy, wz, wpx, wy, wpz, wx, wy, wpz, utils::WHITE, 0.0f, -1.0f, 0.0f, getBlockTextureForFace(block, Face::Bottom));
                }
            }
        }
    }


 }
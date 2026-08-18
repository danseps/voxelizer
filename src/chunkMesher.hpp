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
 #include "world.hpp"
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
        float ao; // Ambient occlusion factor
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
     * @brief Get the Block object type from a chunk based on its global 3D coordinates. This function takes into account the world and the chunk's position within it.
     * 
     * @param world 
     * @param globalX 
     * @param globalY 
     * @param globalZ 
     * @return BlockType 
     */
    inline BlockType getBlock(World& world, int globalX, int globalY, int globalZ)
    {
        // Ochrana proti čtení pod mapou a nad mapou (nebe a void)
        if (globalY < 0 || globalY >= Chunk::SIZE_Y) {
            return BlockType::Air;
        }
 
        // Zjistíme, do jakého chunku tyhle globální souřadnice padnou
        int chunkX = static_cast<int>(std::floor(globalX / static_cast<float>(Chunk::SIZE_X)));
        int chunkZ = static_cast<int>(std::floor(globalZ / static_cast<float>(Chunk::SIZE_Z)));
 
        // Pokud svět tento chunk ještě nenačetl, vrátíme vzduch (aby nevznikla neviditelná zeď)
        if (!world.hasChunk(chunkX, chunkZ)) {
            return BlockType::Air;
        }
 
        // Vypočítáme lokální souřadnice bloku uvnitř nalezeného chunku
        int localX = globalX - (chunkX * Chunk::SIZE_X);
        int localZ = globalZ - (chunkZ * Chunk::SIZE_Z);
 
        // Získáme ten správný chunk ze světa a zavoláme naši rychlou lokální funkci!
        const Chunk& targetChunk = world.getChunk(chunkX, chunkZ);
        return getBlockLocal(targetChunk, localX, globalY, localZ);
    }

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
                        float x0, float y0, float z0, float ao0,
                        float x1, float y1, float z1, float ao1,
                        float x2, float y2, float z2, float ao2,
                        float x3, float y3, float z3, float ao3,
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

            meshData.vertices.push_back({x0, y0, z0, color, nx, ny, nz, u0, v0, ao0});
            meshData.vertices.push_back({x1, y1, z1, color, nx, ny, nz, u1, v0, ao1});
            meshData.vertices.push_back({x2, y2, z2, color, nx, ny, nz, u1, v1, ao2});
            meshData.vertices.push_back({x3, y3, z3, color, nx, ny, nz, u0, v1, ao3});

            // Determine the diagonal split based on ambient occlusion values
            // Quad splitting
            if (ao0 + ao2 < ao1 + ao3) {
                // Diagonal split: 0-1-2 and 2-3-0
                meshData.indices.push_back(offset + 0);
                meshData.indices.push_back(offset + 1);
                meshData.indices.push_back(offset + 2);
                meshData.indices.push_back(offset + 2);
                meshData.indices.push_back(offset + 3);
                meshData.indices.push_back(offset + 0);
            } else {
                // Diagonal split: 1-2-3 and 3-0-1
                meshData.indices.push_back(offset + 1);
                meshData.indices.push_back(offset + 2);
                meshData.indices.push_back(offset + 3);
                meshData.indices.push_back(offset + 3);
                meshData.indices.push_back(offset + 0);
                meshData.indices.push_back(offset + 1);
            }
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
                    return tileUV(14, 14);
                if (face == Face::Bottom)
                    return tileUV(2, 15);
                return tileUV(3, 15);
            default:
                return tileUV(0, 0);
        }
    }

    /**
     * @brief 
     * 
     * @param world 
     * @param globalX 
     * @param globalY 
     * @param globalZ 
     * @return true 
     * @return false 
     */
    inline bool isSolid(World& world, int globalX, int globalY, int globalZ) {
        return getBlock(world, globalX, globalY, globalZ) != BlockType::Air;
    }

    /**
     * @brief Get the Vertex A O object
     * 
     * @param side1 
     * @param side2 
     * @param corner 
     * @return float based on neighboring blocks, where 0 is full shadow and 1 is full light
     */
    inline float getVertexAO(bool side1, bool side2, bool corner) {
        // More obvious AO: fully occluded corners become significantly darker,
        // while open corners remain near full brightness.
        int occluded = 0;
        if (side1) occluded++;
        if (side2) occluded++;
        if (corner) occluded++;

        if (occluded >= 3) return 0.25f;
        if (occluded == 2) return 0.45f;
        if (occluded == 1) return 0.7f;
        return 1.0f;
    }

    /**
     * @brief Called when drawing a face, calculates the ambient occlusion for each vertex of that face based on neighboring blocks.
     * 
     * @param chunk 
     * @param x 
     * @param y 
     * @param z 
     * @param face 
     * @param ao0 
     * @param ao1 
     * @param ao2 
     * @param ao3 
     */
    inline void calculateFaceAO(World& world, int gx, int gy, int gz, Face face, 
                                float& ao0, float& ao1, float& ao2, float& ao3)
    {
        bool s1, s2, c;

        switch (face)
        {
            case Face::Front: // +Z stěna
            {
                int pz = gz + 1;
                // Levý dolní roh (v0)
                s1 = isSolid(world, gx - 1, gy, pz); s2 = isSolid(world, gx, gy - 1, pz); c = isSolid(world, gx - 1, gy - 1, pz);
                ao0 = getVertexAO(s1, s2, c);
                // Pravý dolní roh (v1)
                s1 = isSolid(world, gx + 1, gy, pz); s2 = isSolid(world, gx, gy - 1, pz); c = isSolid(world, gx + 1, gy - 1, pz);
                ao1 = getVertexAO(s1, s2, c);
                // Pravý horní roh (v2)
                s1 = isSolid(world, gx + 1, gy, pz); s2 = isSolid(world, gx, gy + 1, pz); c = isSolid(world, gx + 1, gy + 1, pz);
                ao2 = getVertexAO(s1, s2, c);
                // Levý horní roh (v3)
                s1 = isSolid(world, gx - 1, gy, pz); s2 = isSolid(world, gx, gy + 1, pz); c = isSolid(world, gx - 1, gy + 1, pz);
                ao3 = getVertexAO(s1, s2, c);
                break;
            }
            case Face::Back: // -Z stěna (pozor na pořadí vrcholů v addFace)
            {
                int mz = gz - 1;
                s1 = isSolid(world, gx + 1, gy, mz); s2 = isSolid(world, gx, gy - 1, mz); c = isSolid(world, gx + 1, gy - 1, mz);
                ao0 = getVertexAO(s1, s2, c);
                
                s1 = isSolid(world, gx - 1, gy, mz); s2 = isSolid(world, gx, gy - 1, mz); c = isSolid(world, gx - 1, gy - 1, mz);
                ao1 = getVertexAO(s1, s2, c);
                
                s1 = isSolid(world, gx - 1, gy, mz); s2 = isSolid(world, gx, gy + 1, mz); c = isSolid(world, gx - 1, gy + 1, mz);
                ao2 = getVertexAO(s1, s2, c);
                
                s1 = isSolid(world, gx + 1, gy, mz); s2 = isSolid(world, gx, gy + 1, mz); c = isSolid(world, gx + 1, gy + 1, mz);
                ao3 = getVertexAO(s1, s2, c);
                break;
            }
            // Zde analogicky doplníš Top, Bottom, Left a Right podle orientace tvých vertexů v addFace...
            case Face::Top: // +Y stěna
            {
                int py = gy + 1; // Hledáme bloky hned nad námi

                // v0: wx, wpz (Levý přední roh -> osa -X, osa +Z)
                s1 = isSolid(world, gx - 1, py, gz);     // Vlevo
                s2 = isSolid(world, gx,     py, gz + 1); // Vpředu
                c  = isSolid(world, gx - 1, py, gz + 1); // Diagonála
                ao0 = getVertexAO(s1, s2, c);

                // v1: wpx, wpz (Pravý přední roh -> osa +X, osa +Z)
                s1 = isSolid(world, gx + 1, py, gz);     // Vpravo
                s2 = isSolid(world, gx,     py, gz + 1); // Vpředu
                c  = isSolid(world, gx + 1, py, gz + 1); // Diagonála
                ao1 = getVertexAO(s1, s2, c);

                // v2: wpx, wz (Pravý zadní roh -> osa +X, osa -Z)
                s1 = isSolid(world, gx + 1, py, gz - 1);     // Vpravo
                s2 = isSolid(world, gx,     py, gz - 1); // Vzadu
                c  = isSolid(world, gx + 1, py, gz - 1); // Diagonála
                ao2 = getVertexAO(s1, s2, c);

                // v3: wx, wz (Levý zadní roh -> osa -X, osa -Z)
                s1 = isSolid(world, gx - 1, py, gz - 1);     // Vlevo
                s2 = isSolid(world, gx,     py, gz - 1); // Vzadu
                c  = isSolid(world, gx - 1, py, gz - 1); // Diagonála
                ao3 = getVertexAO(s1, s2, c);
                break;
            }
            case Face::Bottom: // -Y stěna
            {
                int my = gy - 1; // Hledáme bloky hned pod námi

                // v0: wx, wz (Levý zadní roh -> osa -X, osa -Z)
                s1 = isSolid(world, gx - 1, my, gz);     // Vlevo
                s2 = isSolid(world, gx,     my, gz - 1); // Vzadu
                c  = isSolid(world, gx - 1, my, gz - 1); // Diagonála
                ao0 = getVertexAO(s1, s2, c);

                // v1: wpx, wz (Pravý zadní roh -> osa +X, osa -Z)
                s1 = isSolid(world, gx + 1, my, gz);     // Vpravo
                s2 = isSolid(world, gx,     my, gz - 1); // Vzadu
                c  = isSolid(world, gx + 1, my, gz - 1); // Diagonála
                ao1 = getVertexAO(s1, s2, c);

                // v2: wpx, wpz (Pravý přední roh -> osa +X, osa +Z)
                s1 = isSolid(world, gx + 1, my, gz);     // Vpravo
                s2 = isSolid(world, gx,     my, gz + 1); // Vpředu
                c  = isSolid(world, gx + 1, my, gz + 1); // Diagonála
                ao2 = getVertexAO(s1, s2, c);

                // v3: wx, wpz (Levý přední roh -> osa -X, osa +Z)
                s1 = isSolid(world, gx - 1, my, gz + 1);     // Vlevo
                s2 = isSolid(world, gx,     my, gz + 1); // Vpředu
                c  = isSolid(world, gx - 1, my, gz + 1); // Diagonála
                ao3 = getVertexAO(s1, s2, c);
                break;
            }
            case Face::Left: // -X stěna
            {
                int mx = gx - 1;
                // Levý dolní roh (v0)
                s1 = isSolid(world, mx, gy, gz - 1); s2 = isSolid(world, mx, gy - 1, gz); c = isSolid(world, mx, gy - 1, gz - 1);
                ao0 = getVertexAO(s1, s2, c);
                // Levý horní roh (v3)
                s1 = isSolid(world, mx, gy, gz + 1); s2 = isSolid(world, mx, gy - 1, gz); c = isSolid(world, mx, gy - 1, gz + 1);
                ao1 = getVertexAO(s1, s2, c);
                // Pravý horní roh (v2)
                s1 = isSolid(world, mx, gy, gz + 1); s2 = isSolid(world, mx, gy + 1, gz); c = isSolid(world, mx, gy + 1, gz + 1);
                ao2 = getVertexAO(s1, s2, c);
                // Pravý dolní roh (v1)
                s1 = isSolid(world, mx, gy, gz - 1); s2 = isSolid(world, mx, gy + 1, gz); c = isSolid(world, mx, gy + 1, gz - 1);
                ao3 = getVertexAO(s1, s2, c);
                break;  
            }
            case Face::Right: // +X stěna
            {
                int px = gx + 1;
                // Levý dolní roh (v0)
                s1 = isSolid(world, px, gy, gz + 1); s2 = isSolid(world, px, gy - 1, gz); c = isSolid(world, px, gy - 1, gz + 1);
                ao0 = getVertexAO(s1, s2, c);
                // Levý horní roh (v3)
                s1 = isSolid(world, px, gy, gz - 1); s2 = isSolid(world, px, gy - 1, gz); c = isSolid(world, px, gy - 1, gz - 1);
                ao1 = getVertexAO(s1, s2, c);
                // Pravý horní roh (v2)
                s1 = isSolid(world, px, gy, gz - 1); s2 = isSolid(world, px, gy + 1, gz); c = isSolid(world, px, gy + 1, gz - 1);
                ao2 = getVertexAO(s1, s2, c);
                // Pravý dolní roh (v1)
                s1 = isSolid(world, px, gy, gz + 1); s2 = isSolid(world, px, gy + 1, gz); c = isSolid(world, px, gy + 1, gz + 1);
                ao3 = getVertexAO(s1, s2, c);
                break;  
            }
            default:
                ao0 = ao1 = ao2 = ao3 = 1.0f;
                break;
        }
    }


    /**
     * @brief Generate a mesh for the given chunk
     * 
     * @param chunk 
     */
    inline void generateMesh(World& world, const Chunk& chunk, int chunkX, int chunkZ, MeshData& meshData)
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
                    BlockType block = getBlockLocal(chunk, x, y, z); // faster
                    if (block == BlockType::Air)
                    {
                        continue;
                    }

                    // Global coordinates in the world (for AO calculations)
                    int gx = x + offsetX;
                    int gy = y;
                    int gz = z + offsetZ;

                    // APLIKACE OFFSETU
                    float wx = x + offsetX;
                    float wy = y; 
                    float wz = z + offsetZ;

                    float wpx = wx + 1.0f;
                    float wpy = wy + 1.0f;
                    float wpz = wz + 1.0f;

                    // A teď už používáme ty posunuté (World) souřadnice (wx, wy, wz)!
                    // 1. PŘEDNÍ STĚNA (+Z)
                    if (!isSolid(world, gx, gy, gz + 1)) //TODO: vytvorit a poslat cely vertex?? misto tolika cisel 
                    {
                        float ao0, ao1, ao2, ao3;
                        calculateFaceAO(world, gx, gy, gz, Face::Front, ao0, ao1, ao2, ao3);

                        addFace(meshData, 
                            wx, wy, wpz, ao0,
                            wpx, wy, wpz, ao1,
                            wpx, wpy, wpz, ao2,
                            wx, wpy, wpz, ao3,
                            utils::WHITE /*TODO: pozdeji budem pouzivat u lightingu ig*/,
                            0.0f, 0.0f, 1.0f, // Normálový vektor pro přední stěnu je (0, 0, 1)
                            getBlockTextureForFace(block, Face::Front)
                        ); 
                    }

                    // 2. ZADNÍ STĚNA (-Z)
                    if (!isSolid(world, gx, gy, gz - 1))
                    {
                        float ao0, ao1, ao2, ao3;
                        calculateFaceAO(world, gx, gy, gz, Face::Back, ao0, ao1, ao2, ao3);
                        addFace(meshData, 
                            wpx, wy, wz, ao0,
                            wx, wy, wz, ao1,
                            wx, wpy, wz, ao2,
                            wpx, wpy, wz, ao3,
                            utils::WHITE, 
                            0.0f, 0.0f, -1.0f, 
                            getBlockTextureForFace(block, Face::Back)
                        );
                    }

                    // 3. LEVÁ STĚNA (-X)
                    if (!isSolid(world, gx - 1, gy, gz))
                    {
                        float ao0, ao1, ao2, ao3;
                        calculateFaceAO(world, gx, gy, gz, Face::Left, ao0, ao1, ao2, ao3);
                        addFace(meshData, 
                            wx, wy, wz, ao0, 
                            wx, wy, wpz, ao1, 
                            wx, wpy, wpz, ao2, 
                            wx, wpy, wz, ao3, 
                            utils::WHITE, 
                            -1.0f, 0.0f, 0.0f, 
                            getBlockTextureForFace(block, Face::Left)
                        );
                    }

                    // 4. PRAVÁ STĚNA (+X)
                    if (!isSolid(world, gx + 1, gy, gz))
                    {
                        float ao0, ao1, ao2, ao3;
                        calculateFaceAO(world, gx, gy, gz, Face::Right, ao0, ao1, ao2, ao3);
                        addFace(meshData, 
                            wpx, wy, wpz, ao0, 
                            wpx, wy, wz, ao1, 
                            wpx, wpy, wz, ao2, 
                            wpx, wpy, wpz, ao3, 
                            utils::WHITE, 
                            1.0f, 0.0f, 0.0f, 
                            getBlockTextureForFace(block, Face::Right)
                        );
                    }

                    // 5. HORNÍ STĚNA (+Y)
                    if (!isSolid(world, gx, gy + 1, gz))
                    {
                        float ao0, ao1, ao2, ao3;
                        calculateFaceAO(world, gx, gy, gz, Face::Top, ao0, ao1, ao2, ao3);
                        addFace(meshData, 
                            wx, wpy, wpz, ao0, 
                            wpx, wpy, wpz, ao1, 
                            wpx, wpy, wz, ao2, 
                            wx, wpy, wz, ao3, 
                            utils::WHITE, 
                            0.0f, 1.0f, 0.0f, 
                            getBlockTextureForFace(block, Face::Top)
                        );
                    }

                    // 6. SPODNÍ STĚNA (-Y)
                    if (!isSolid(world, gx, gy - 1, gz))
                    {
                        float ao0, ao1, ao2, ao3;
                        calculateFaceAO(world, gx, gy, gz, Face::Bottom, ao0, ao1, ao2, ao3);
                        addFace(meshData, 
                            wx, wy, wz, ao0, 
                            wpx, wy, wz, ao1, 
                            wpx, wy, wpz, ao2, 
                            wx, wy, wpz, ao3, 
                            utils::WHITE, 
                            0.0f, -1.0f, 0.0f, 
                            getBlockTextureForFace(block, Face::Bottom)
                        );
                    }
                }
            }
        }
    }
 };
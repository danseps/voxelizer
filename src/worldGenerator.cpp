/**
 * @file worldGenerator.cpp
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2026-07-18
 * 
 * @copyright Copyright (c) 2026
 * 
 */
#include "worldGenerator.hpp"
#include "perlinNoise.hpp"
#include "chunkMesher.hpp"

#include <iostream>

void generateChunk(Chunk& chunk, int offsetX, int offsetZ)
{
    for (int z = 0; z < Chunk::SIZE_Z; z++)
    {
        float globalZ = offsetZ * Chunk::SIZE_Z + z;
        for (int x = 0; x < Chunk::SIZE_X; x++)
        {
            float globalX = offsetX * Chunk::SIZE_X + x;

            float noiseVal = octavePerlin(globalX * 0.05f, globalZ * 0.05f, 12, 0.5f);
            // 2. Šum je od -1.0 do 1.0. Upravíme to matematicky na 0.0 až 1.0.
            float normalized = (noiseVal + 1.0f) / 2.0f;
            
            // 3. Převedeme to na výšku kopce v blocích (např. od 2 do 14 bloků vysoko)
            int height = 2 + static_cast<int>(normalized * (Chunk::SIZE_Y - 4));

            //TODO: debug
            if (height < 0 || height > Chunk::SIZE_Y) {
            printf("VAROVÁNÍ: Špatná výška: %d pro X:%f Z:%f\n", height, globalX, globalZ);
            height = 5; // Bezpečná hodnota
        }
            
            for (int y = 0; y < height; y++) {
                
                // TODO: zmenit, zatim jen podle vysky
                if (y == height - 1) {
                    setBlock(chunk, x, y, z, BlockType::Grass);
                } else if (y >= height - 4) {
                    setBlock(chunk, x, y, z, BlockType::Dirt);
                } else {
                    setBlock(chunk, x, y, z, BlockType::Stone);
                }
            }

        }
        
    }
}


//TODO: generova chunky pro nejaky radius
void generateWorld(World& world)
{
    // For demonstration, let's generate a 3x3 grid of chunks
    for (int chunkX = -1; chunkX <= 1; ++chunkX)
    {
        for (int chunkZ = -1; chunkZ <= 1; ++chunkZ)
        {
            Chunk& chunk = world.getChunk(chunkX, chunkZ);
            generateChunk(chunk, chunkX, chunkZ);
            
        }
    }
}

void generateWorldMesh(World& world, ChunkMesher::MeshData& mesh)
{
    for (const auto& [coord, chunk] : world.getChunkMap())
    {
        //TODO: generateMesh(chunk)
        ChunkMesher::generateMesh(chunk, coord.x, coord.z, mesh);
    }
}
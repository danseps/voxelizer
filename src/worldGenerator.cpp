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
    constexpr int TERRAIN_BASE = 2;
    constexpr int MAX_TERRAIN_HEIGHT = 32; // Maximum height of the terrain above the base level, TODO:adjust as needed

    for (int z = 0; z < Chunk::SIZE_Z; z++)
    {
        float globalZ = offsetZ * Chunk::SIZE_Z + z;
        for (int x = 0; x < Chunk::SIZE_X; x++)
        {
            float globalX = offsetX * Chunk::SIZE_X + x;

            float noiseVal = octavePerlin(globalX * 0.05f, globalZ * 0.05f, 12, 0.5f);
            float normalized = (noiseVal + 1.0f) / 2.0f;

            int height = TERRAIN_BASE + static_cast<int>(normalized * MAX_TERRAIN_HEIGHT);
            height = std::max(1, std::min(height, static_cast<int>(Chunk::SIZE_Y - 1)));

            for (int y = 0; y < height; y++)
            {
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
    for (int chunkX = -3; chunkX <= 3; ++chunkX)
    {
        for (int chunkZ = -3; chunkZ <= 3; ++chunkZ)
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
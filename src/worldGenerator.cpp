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

void generateWorldMesh(World& world, std::unordered_map<ChunkCoord, Mesh*, ChunkCoordHash>& chunkMeshes)
{
    for (const auto& [coord, chunk] : world.getChunkMap())
    {
        // Create a new Mesh for each chunk and generate its mesh data
        Mesh* mesh = new Mesh();
        ChunkMesher::MeshData chunkMeshData;
        ChunkMesher::generateMesh(world, chunk, coord.x, coord.z, chunkMeshData);
        mesh->uploadData(chunkMeshData); // Upload the generated mesh data to GPU buffers
        chunkMeshes[coord] = mesh; // Store the mesh in the map
    }
}

void updateChunks(World& world, std::unordered_map<ChunkCoord, Mesh*, ChunkCoordHash>& chunkMeshes, const glm::vec3& cameraPos)
{
    //TODO: changeable
    int renderDistance = 4; // Number of chunks to render in each direction from the camera
    
    // Calculate the chunk coordinates of the camera position
    glm::ivec2 myChunkCoord = {
        std::floor(cameraPos.x / Chunk::SIZE_X),
        std::floor(cameraPos.z / Chunk::SIZE_Z)
    };

    // Determine which chunks should be present based on the camera position and render distance
    
    // ---------------------------------------------------------
    // FÁZE 1: Generování dat (RAM) + Límec (renderDistance + 1)
    // ---------------------------------------------------------
    for (int x = myChunkCoord.x - (renderDistance + 1); x <= myChunkCoord.x + (renderDistance + 1); x++)
    {
        for (int z = myChunkCoord.y - (renderDistance + 1); z <= myChunkCoord.y + (renderDistance + 1); z++)
        {
            if (!world.hasChunk(x, z))
            {
                // Generate the chunk purely to RAM
                Chunk& newChunk = world.getChunk(x, z);
                generateChunk(newChunk, x, z);
            }
        }
    }

    // ---------------------------------------------------------
    // FÁZE 2: Generování 3D sítě (VRAM) (pouze renderDistance)
    // ---------------------------------------------------------
    for (int x = myChunkCoord.x - renderDistance; x <= myChunkCoord.x + renderDistance; x++)
    {
        for (int z = myChunkCoord.y - renderDistance; z <= myChunkCoord.y + renderDistance; z++)
        {
            ChunkCoord coord{x, z};

            // Pokud ještě nemáme Mesh pro tento chunk
            if(chunkMeshes.find(coord) == chunkMeshes.end())
            {
                Chunk& chunk = world.getChunk(x, z);
                Mesh* mesh = new Mesh();
                ChunkMesher::MeshData chunkMeshData;
                
                // Mesher se teď může bezpečně podívat za hranice, protože Fáze 1 tam už data připravila
                ChunkMesher::generateMesh(world, chunk, x, z, chunkMeshData);
                
                mesh->uploadData(chunkMeshData);
                chunkMeshes[coord] = mesh;
            }
        }
    }

    // Remove chunks that are outside the render distance
    for (auto it = chunkMeshes.begin(); it != chunkMeshes.end();)
    {
        ChunkCoord coord = it->first;
        Mesh* mesh = it->second;

        if (std::abs(coord.x - myChunkCoord.x) > (renderDistance + 1) || 
            std::abs(coord.z - myChunkCoord.y) > (renderDistance + 1))
        {
            // Delete the mesh from VRAM and remove it from the map
            delete mesh;
            it = chunkMeshes.erase(it); // Remove the mesh from the map and get the next iterator
            //world.deleteChunk(coord.x, coord.z);
        }
        else
        {
            // The chunk is within the render distance, so we keep it
            ++it;
        }
    }

}
/**
 * @file world.hpp
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2026-07-18
 * 
 * @copyright Copyright (c) 2026
 * 
 */

 #pragma once

#include "chunk.hpp"
#include <unordered_map>
#include <functional> // For std::hash

struct ChunkCoord
{
    int x;
    int z;

    bool operator==(const ChunkCoord& other) const
    {
        // Two ChunkCoord instances are equal when both their x and z
        // coordinates match. This operator is used by associative
        // containers (like unordered_map) to compare keys.
        return x == other.x && z == other.z;
    }
};

struct ChunkCoordHash
{
    std::size_t operator()(const ChunkCoord& coord) const
    {
        // Combine the hash of x and z coordinates to produce a unique hash
        // for the ChunkCoord. This is used by unordered_map to efficiently
        // store and retrieve chunks based on their coordinates.
        return std::hash<int>()(coord.x) ^ (std::hash<int>()(coord.z) << 1);
    }
};

 class World
 {
private:
    std::unordered_map<ChunkCoord, Chunk, ChunkCoordHash> chunks; // Map of chunk coordinates to chunks

public:
    /**
     * @brief Get a reference to the chunk at the specified coordinates. If the chunk does not exist, it will be created and initialized to default values.
     */
    Chunk& getChunk(int chunkX, int chunkZ)
    {
        ChunkCoord coord{chunkX, chunkZ};
        // in c++17, the operator[] of unordered_map will create a new entry if the key does not exist.
        return chunks[coord];
    }

    int getSize()
    {
        return chunks.size();
    }

    std::unordered_map<ChunkCoord, Chunk, ChunkCoordHash> getChunkMap()
    {
        return chunks;
    }

    /**
     * @brief Check if a chunk exists at the specified coordinates.
     * @param chunkX The x-coordinate of the chunk.
     * @param chunkZ The z-coordinate of the chunk.
     * @return True if the chunk exists, false otherwise.
     */
    bool hasChunk(int chunkX, int chunkZ) const
    {
        ChunkCoord coord{chunkX, chunkZ};
        return chunks.find(coord) != chunks.end();
    }
 };
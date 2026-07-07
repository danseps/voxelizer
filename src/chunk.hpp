/**
 * @file chunk.hpp
 * @author Daniel Šeps (daniel.seps@proton.me)
 * @brief 
 * @version 0.1
 * @date 2026-07-06
 * 
 * @copyright Copyright (c) 2026
 * 
 */

#pragma once

#include <cstdint>

enum class BlockType : uint8_t
{
    Air,
    Grass,
    Dirt,
    Stone,
    Water,
    Sand,
    Wood,
    Leaves,
    Snow,
    Lava
};

/**
 * @brief Represents a chunk of the world (16x16x16 blocks)
 * 
 */
struct Chunk
{
    static constexpr uint8_t SIZE_X = 16; 
    static constexpr uint8_t SIZE_Y = 16; 
    static constexpr uint8_t SIZE_Z = 16; 
    static constexpr int VOLUME = SIZE_X * SIZE_Y * SIZE_Z;
    BlockType blocks[VOLUME];
};

/**
 * @brief Get the index of a block in the chunk's 1D block array based on its 3D coordinates
 * 
 * @param x 
 * @param y 
 * @param z 
 * @return int 
 */

inline int getBlockIndex(int x, int y, int z)
{
    if (x < 0 || x >= Chunk::SIZE_X || 
        y < 0 || y >= Chunk::SIZE_Y || 
        z < 0 || z >= Chunk::SIZE_Z) {
        return -1; 
    }
    return x + (y * Chunk::SIZE_X) + (z * Chunk::SIZE_X * Chunk::SIZE_Y);
}

inline void setBlock(Chunk& chunk, int x, int y, int z, BlockType blockType)
{
    int index = getBlockIndex(x, y, z);
    if (index != -1) {
        chunk.blocks[index] = blockType;
    }
}

inline BlockType getBlock(Chunk& chunk, int x, int y, int z)
{
    int index = getBlockIndex(x, y, z);
    if (index != -1) {
        return chunk.blocks[index];
    }
    return BlockType::Air;
}
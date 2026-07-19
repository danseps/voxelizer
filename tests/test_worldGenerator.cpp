/**
 * @file test_worldGenerator.cpp
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2026-07-19
 * 
 * @copyright Copyright (c) 2026
 * 
 */

 #include <gtest/gtest.h>
#include "../src/chunk.hpp"
#include "../src/worldGenerator.hpp"
#include <cmath>

TEST(WorldGenTest, HeightIsAlwaysWithinSafeBounds) {
    Chunk chunk{};
    generateChunk(chunk, 0, 0);

    // middle of the chunk, check the height of the block at (5, ?, 5)
    int topBlockY = -1;
    for (int y = Chunk::SIZE_Y - 1; y >= 0; y--) {
        if (getBlock(chunk, 5, y, 5) != BlockType::Air) {
            topBlockY = y;
            break;
        }
    }

    // hard coded limits
    EXPECT_GE(topBlockY, 1); 
    EXPECT_LE(topBlockY, Chunk::SIZE_Y - 2); 
}

TEST(WorldGenTest, ChunksConnectSeamlessly) {
    Chunk chunkLeft{};
    Chunk chunkRight{};

    // chunks next to each other
    generateChunk(chunkLeft, 0, 0);
    generateChunk(chunkRight, 1, 0);

    int heightLeft = 0;
    while(heightLeft < Chunk::SIZE_Y && getBlock(chunkLeft, Chunk::SIZE_X - 1, heightLeft, 5) != BlockType::Air) {
        heightLeft++;
    }

    int heightRight = 0;
    while(heightRight < Chunk::SIZE_Y && getBlock(chunkRight, 0, heightRight, 5) != BlockType::Air) {
        heightRight++;
    }

    // Height diff between the two chunks shuld be minimal (e.g., less than or equal to 2 blocks)
    int difference = std::abs(heightLeft - heightRight);
    EXPECT_LE(difference, 2); 
}
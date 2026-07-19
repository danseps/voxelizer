/**
 * @file test_world.cpp
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2026-07-19
 * 
 * @copyright Copyright (c) 2026
 * 
 */

#include <gtest/gtest.h>
#include "../src/world.hpp"

TEST(WorldTest, IsEmptyInitially) {
    World world;
    EXPECT_FALSE(world.hasChunk(0, 0));
    EXPECT_FALSE(world.hasChunk(10, -5));
    EXPECT_EQ(world.getChunkMap().size(), 0);
}

TEST(WorldTest, GetChunkCreatesChunkIfMissing) {
    World world;
    
    Chunk& chunk = world.getChunk(0, 0);
    
    EXPECT_TRUE(world.hasChunk(0, 0));
    EXPECT_EQ(world.getChunkMap().size(), 1);
    
    EXPECT_EQ(getBlock(chunk, 0, 0, 0), BlockType::Air);
}

TEST(WorldTest, MultipleChunksStoredSafely) {
    World world;
    
    world.getChunk(0, 0);
    world.getChunk(1, 0);
    world.getChunk(-1, -1);
    
    EXPECT_EQ(world.getChunkMap().size(), 3);
    EXPECT_TRUE(world.hasChunk(-1, -1));
    EXPECT_FALSE(world.hasChunk(1, 1)); // not created (CHUNK 2d map coords not world coords)
}
#include <gtest/gtest.h>
#include "../src/chunk.hpp"

// Všechny testy, které použijí makro TEST_F místo TEST, budou mít přístup k proměnným zde uvnitř.
class ChunkTest : public ::testing::Test {
protected:
    Chunk chunk{};
};

TEST_F(ChunkTest, IndexInBounds) {
    EXPECT_EQ(getBlockIndex(0,0,0), 0);
    EXPECT_EQ(getBlockIndex(15, 15, 15), 4095); // 16*16*16 - 1

    // Middle
    EXPECT_EQ(getBlockIndex(1,0,0), 1);
    EXPECT_EQ(getBlockIndex(0,1,0), 16);
    EXPECT_EQ(getBlockIndex(0,0,1), 256);
}

TEST_F(ChunkTest, IndexOutOfBounds)
{
    EXPECT_EQ(getBlockIndex(-1, 5, 5), -1);
    EXPECT_EQ(getBlockIndex(16, 5, 5), -1);
    EXPECT_EQ(getBlockIndex(5, 5, 20), -1);   
}

TEST_F(ChunkTest, DefaultStateIsAir) {
    EXPECT_EQ(getBlock(chunk, 8, 8, 8), BlockType::Air);
    EXPECT_EQ(getBlock(chunk, 0, 0, 0), BlockType::Air);
}

TEST_F(ChunkTest, SetBlockChangesData) {
    setBlock(chunk, 8, 8, 8, BlockType::Dirt);
    EXPECT_EQ(getBlock(chunk, 8, 8, 8), BlockType::Dirt);
    
    EXPECT_EQ(getBlock(chunk, 8, 8, 9), BlockType::Air);
}

TEST_F(ChunkTest, OutOfBoundsGetSet) {
    EXPECT_EQ(getBlock(chunk, 50, 50, 50), BlockType::Air);
    
    // ignored
    setBlock(chunk, -1, 0, 0, BlockType::Stone);
    EXPECT_EQ(getBlock(chunk, -1, 0, 0), BlockType::Air);
}
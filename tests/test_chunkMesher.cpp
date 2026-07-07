/**
 * @file test_chunkMesher.cpp
 * @author Daniel Šeps (daniel.seps@proton.me)
 * @brief ChunkMesher test file
 * @version 0.1
 * @date 2026-07-07
 * 
 * @copyright Copyright (c) 2026
 * 
 */
#include <gtest/gtest.h>
#include "../src/chunkMesher.hpp"

TEST(ChunkMesherTest, GenerateMeshEmptyChunk) {
    Chunk chunk{};
    auto mesh = ChunkMesher::generateMesh(chunk);
    EXPECT_TRUE(mesh.empty());
}

TEST(ChunkMesherTest, GenerateMeshSingleBlock) {
    Chunk chunk{};
    setBlock(chunk, 0, 0, 0, BlockType::Dirt);
    auto mesh = ChunkMesher::generateMesh(chunk);
    EXPECT_FALSE(mesh.empty());
    EXPECT_EQ(mesh.size(), 36 * 7); // 6 faces * 2 triangles * 3 vertices * (x,y,z,r,g,b,a)
}

TEST(ChunkMesherTest, ColorCheck) {
    Chunk chunk{};
    setBlock(chunk, 0, 0, 0, BlockType::Grass);
    auto mesh = ChunkMesher::generateMesh(chunk);
    utils::Color expectedColor = ChunkMesher::getBlockTypeColor(BlockType::Grass);
    EXPECT_FLOAT_EQ(mesh[3], expectedColor.r);
    EXPECT_FLOAT_EQ(mesh[4], expectedColor.g);
    EXPECT_FLOAT_EQ(mesh[5], expectedColor.b);
    EXPECT_FLOAT_EQ(mesh[6], expectedColor.a);
}

TEST(ChunkMesherTest, GenerateMeshMultipleBlocks) {
    Chunk chunk{};
    setBlock(chunk, 0, 0, 0, BlockType::Dirt);
    setBlock(chunk, 1, 0, 0, BlockType::Stone);
    auto mesh = ChunkMesher::generateMesh(chunk);
    EXPECT_FALSE(mesh.empty());
    EXPECT_EQ(mesh.size(), 2 * 36 * 7); // 2 blocks * 6 faces * 2 triangles * 3 vertices * (x,y,z,r,g,b,a)
}

TEST(ChunkMesherTest, GenerateMeshWithAirBlocks) {
    Chunk chunk{};
    setBlock(chunk, 0, 0, 0, BlockType::Dirt);
    setBlock(chunk, 1, 0, 0, BlockType::Air); // This block should not contribute to the mesh
    auto mesh = ChunkMesher::generateMesh(chunk);
    EXPECT_FALSE(mesh.empty());
    EXPECT_EQ(mesh.size(), 36 * 7); // Only the Dirt block contributes to the mesh
}

TEST(ChunkMesherTest, GeometryCoordinatesCheck)
{
    Chunk chunk{};
    setBlock(chunk, 0, 0, 0, BlockType::Dirt);
    auto mesh = ChunkMesher::generateMesh(chunk);
    
    // Check the first vertex coordinates
    EXPECT_FLOAT_EQ(mesh[0], 0.0f); // x
    EXPECT_FLOAT_EQ(mesh[1], 0.0f); // y
    EXPECT_FLOAT_EQ(mesh[2], 1.0f); // z (front face)
}

//TODO: Face culling test: Ensure that blocks that are 
// completely surrounded by other blocks do not generate faces in the mesh.
/*
    Myšlenka: Dáme dvě kostky Hlíny těsně vedle sebe: (0,0,0) a (1,0,0).
V současném hrubém mesheru se vygenerují 
obě celé kostky, takže získáme 504 floatů (2 * 252).

Až zapneme Face Culling, tento test upravíme tak, 
aby očekával menší číslo (o 2 skryté stěny méně),
 čímž získáme neprůstřelný důkaz, že nám optimalizace funguje.
*/
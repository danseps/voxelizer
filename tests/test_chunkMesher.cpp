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

class ChunkMesherTest : public ::testing::Test {
protected:
    Chunk chunk{}; // Automaticky vynulovaný (prázdný) chunk pro každý test
};

TEST_F(ChunkMesherTest, GenerateMeshEmptyChunk) {
    auto mesh = ChunkMesher::generateMesh(chunk);

    EXPECT_TRUE(mesh.vertices.empty());
    EXPECT_TRUE(mesh.indices.empty());
}

TEST_F(ChunkMesherTest, GenerateMeshSingleBlock) {
    setBlock(chunk, 0, 0, 0, BlockType::Dirt);
    auto mesh = ChunkMesher::generateMesh(chunk);

    EXPECT_FALSE(mesh.vertices.empty());
    EXPECT_FALSE(mesh.indices.empty());
    EXPECT_EQ(mesh.vertices.size(), 24); // 6 faces * 4 vertices
    EXPECT_EQ(mesh.indices.size(), 36);  // 6 faces * 2 triangles * 3 indices
}

TEST_F(ChunkMesherTest, ColorCheck) {
    setBlock(chunk, 0, 0, 0, BlockType::Grass);
    auto mesh = ChunkMesher::generateMesh(chunk);

    utils::Color expectedColor = ChunkMesher::getBlockTypeColor(BlockType::Grass);
    ASSERT_FALSE(mesh.vertices.empty());
    EXPECT_FLOAT_EQ(mesh.vertices[0].color.r, expectedColor.r);
    EXPECT_FLOAT_EQ(mesh.vertices[0].color.g, expectedColor.g);
    EXPECT_FLOAT_EQ(mesh.vertices[0].color.b, expectedColor.b);
    EXPECT_FLOAT_EQ(mesh.vertices[0].color.a, expectedColor.a);
}

TEST_F(ChunkMesherTest, GenerateMeshMultipleBlocks) {
    setBlock(chunk, 0, 0, 0, BlockType::Dirt);
    setBlock(chunk, 1, 0, 0, BlockType::Stone);
    auto mesh = ChunkMesher::generateMesh(chunk);

    EXPECT_FALSE(mesh.vertices.empty());
    EXPECT_FALSE(mesh.indices.empty());
    EXPECT_EQ(mesh.vertices.size(), 2 * 24); // 2 blocks * 6 faces * 4 vertices
    EXPECT_EQ(mesh.indices.size(), 2 * 36);  // 2 blocks * 6 faces * 2 triangles * 3 indices
}

TEST_F(ChunkMesherTest, GenerateMeshWithAirBlocks) {
    setBlock(chunk, 0, 0, 0, BlockType::Dirt);
    setBlock(chunk, 1, 0, 0, BlockType::Air); // This block should not contribute to the mesh
    auto mesh = ChunkMesher::generateMesh(chunk);

    EXPECT_FALSE(mesh.vertices.empty());
    EXPECT_FALSE(mesh.indices.empty());
    EXPECT_EQ(mesh.vertices.size(), 24); // Only the Dirt block contributes to the mesh
    EXPECT_EQ(mesh.indices.size(), 36);
}

TEST_F(ChunkMesherTest, GeometryCoordinatesCheck)
{
    setBlock(chunk, 0, 0, 0, BlockType::Dirt);
    setBlock(chunk, 15, 15, 15, BlockType::Stone); // Test the farthest corner of the chunk
    auto mesh = ChunkMesher::generateMesh(chunk);
    ASSERT_FALSE(mesh.vertices.empty());
    
    // Check the first vertex coordinates
    EXPECT_FLOAT_EQ(mesh.vertices[0].x, 0.0f); // x
    EXPECT_FLOAT_EQ(mesh.vertices[0].y, 0.0f); // y
    EXPECT_FLOAT_EQ(mesh.vertices[0].z, 1.0f); // z (front face)

    // Check the last vertex coordinates (for the Stone block)
    EXPECT_FLOAT_EQ(mesh.vertices[mesh.vertices.size() - 1].x, 15.0f); // x
    EXPECT_FLOAT_EQ(mesh.vertices[mesh.vertices.size() - 1].y, 15.0f); // y
    EXPECT_FLOAT_EQ(mesh.vertices[mesh.vertices.size() - 1].z, 16.0f); // z
}

TEST_F(ChunkMesherTest, GeometryCoordinatesOutOfBoundsCheck)
{
    setBlock(chunk, 16, 16, 16, BlockType::Stone); 
    
    auto mesh = ChunkMesher::generateMesh(chunk);
    
    // Silently ignore out-of-bounds blocks, so the mesh should be empty, 
    // the chunk is not modified, and no vertices or indices should be generated.
    EXPECT_TRUE(mesh.vertices.empty());
    EXPECT_TRUE(mesh.indices.empty());
    
    EXPECT_EQ(mesh.vertices.size(), 0);
    EXPECT_EQ(mesh.indices.size(), 0);
}

/**
 * @brief Construct a new test f object
 * 
 */
TEST_F(ChunkMesherTest, FirstFaceIndicesAreCorrect) {
    setBlock(chunk, 0, 0, 0, BlockType::Stone);
    
    ChunkMesher::MeshData mesh = ChunkMesher::generateMesh(chunk);
    
    ASSERT_GE(mesh.indices.size(), 6);
    
    // 0, 1, 2, 2, 3, 0 expected for the first face (front face)
    EXPECT_EQ(mesh.indices[0], 0);
    EXPECT_EQ(mesh.indices[1], 1);
    EXPECT_EQ(mesh.indices[2], 2);
    
    EXPECT_EQ(mesh.indices[3], 2);
    EXPECT_EQ(mesh.indices[4], 3);
    EXPECT_EQ(mesh.indices[5], 0);
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
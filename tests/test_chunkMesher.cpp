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
#include "../src/world.hpp"
#include <math.h>

class ChunkMesherTest : public ::testing::Test {
protected:
    World world{}; // Nyní potřebujeme celý svět, ne jen jeden chunk
    ChunkMesher::MeshData mesh;

    // Pomocná funkce pro rychlé nastavení bloku na globálních souřadnicích
    void setWorldBlock(int globalX, int globalY, int globalZ, BlockType type) {
        int chunkX = static_cast<int>(std::floor(globalX / static_cast<float>(Chunk::SIZE_X)));
        int chunkZ = static_cast<int>(std::floor(globalZ / static_cast<float>(Chunk::SIZE_Z)));
        
        int localX = globalX - (chunkX * Chunk::SIZE_X);
        int localZ = globalZ - (chunkZ * Chunk::SIZE_Z);
        
        Chunk& chunk = world.getChunk(chunkX, chunkZ);
        setBlock(chunk, localX, globalY, localZ, type);
    }
};

TEST_F(ChunkMesherTest, GenerateMeshEmptyChunk) {
    const Chunk& chunk = world.getChunk(0, 0);
    ChunkMesher::generateMesh(world, chunk, 0, 0, mesh);

    EXPECT_TRUE(mesh.vertices.empty());
    EXPECT_TRUE(mesh.indices.empty());
}

TEST_F(ChunkMesherTest, GenerateMeshSingleBlock) {
    setWorldBlock(0, 0, 0, BlockType::Dirt);
    
    const Chunk& chunk = world.getChunk(0, 0);
    ChunkMesher::generateMesh(world, chunk, 0, 0, mesh);

    EXPECT_FALSE(mesh.vertices.empty());
    EXPECT_FALSE(mesh.indices.empty());
    EXPECT_EQ(mesh.vertices.size(), 24); // 6 faces * 4 vertices
    EXPECT_EQ(mesh.indices.size(), 36);  // 6 faces * 2 triangles * 3 indices
}

TEST_F(ChunkMesherTest, GenerateMeshMultipleBlocks) {
    setWorldBlock(0, 0, 0, BlockType::Dirt);
    setWorldBlock(1, 0, 0, BlockType::Stone);
    
    const Chunk& chunk = world.getChunk(0, 0);
    ChunkMesher::generateMesh(world, chunk, 0, 0, mesh);

    EXPECT_FALSE(mesh.vertices.empty());
    EXPECT_FALSE(mesh.indices.empty());
    EXPECT_EQ(mesh.vertices.size(), 40); // 10 faces * 4 vertices (1 stěna se skryje = Face Culling)
    EXPECT_EQ(mesh.indices.size(), 60);  // 10 faces * 2 triangles * 3 indices
}

TEST_F(ChunkMesherTest, GenerateMeshWithAirBlocks) {
    setWorldBlock(0, 0, 0, BlockType::Dirt);
    setWorldBlock(1, 0, 0, BlockType::Air); // Vzduch se nesmí modelovat
    
    const Chunk& chunk = world.getChunk(0, 0);
    ChunkMesher::generateMesh(world, chunk, 0, 0, mesh);

    EXPECT_FALSE(mesh.vertices.empty());
    EXPECT_FALSE(mesh.indices.empty());
    EXPECT_EQ(mesh.vertices.size(), 24); // Jen ten jeden blok
    EXPECT_EQ(mesh.indices.size(), 36);
}

TEST_F(ChunkMesherTest, GeometryCoordinatesCheck)
{
    setWorldBlock(0, 0, 0, BlockType::Dirt);
    setWorldBlock(15, 15, 15, BlockType::Stone); // Nejvzdálenější roh uvnitř chunku(0,0)
    
    const Chunk& chunk = world.getChunk(0, 0);
    ChunkMesher::generateMesh(world, chunk, 0, 0, mesh);
    
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

/*TEST_F(ChunkMesherTest, FirstFaceIndicesAreCorrect) {
    setWorldBlock(0, 0, 0, BlockType::Stone);
    
    const Chunk& chunk = world.getChunk(0, 0);
    ChunkMesher::generateMesh(world, chunk, 0, 0, mesh);
    
    ASSERT_GE(mesh.indices.size(), 6);
    
    // 0, 1, 2, 2, 3, 0 expected for the first face (front face)
    EXPECT_EQ(mesh.indices[0], 0);
    EXPECT_EQ(mesh.indices[1], 1);
    EXPECT_EQ(mesh.indices[2], 2);
    
    EXPECT_EQ(mesh.indices[3], 2) || EXPECT_EQ(mesh.indices...; //TODO: quad splitting
    EXPECT_EQ(mesh.indices[4], 3);
    EXPECT_EQ(mesh.indices[5], 0);
}*/

TEST_F(ChunkMesherTest, FaceCullingCompletelySurroundedBlock) {
    // 3x3x3 cube of blocks, the center block is completely surrounded
    for (int y = 0; y < 3; y++) {
        for (int z = 0; z < 3; z++) {
            for (int x = 0; x < 3; x++) {
                setWorldBlock(x, y, z, BlockType::Stone);
            }
        }
    }

    const Chunk& chunk = world.getChunk(0, 0);
    ChunkMesher::generateMesh(world, chunk, 0, 0, mesh);

    // 3x3x3 kostka má navenek 6 velkých stěn. Každá velká stěna se skládá z 3x3 menších.
    // 6 * 9 = 54 stěn celkem. Všechny vnitřní musí zmizet.
    EXPECT_EQ(mesh.vertices.size(), 54 * 4); 
    EXPECT_EQ(mesh.indices.size(), 54 * 6);  
}

// ---------------------------------------------------------
// NOVÉ TESTY: Testování chování na hranicích chunků
// ---------------------------------------------------------

TEST_F(ChunkMesherTest, FaceCullingAtChunkBorder) {
    // Blok na pravém kraji Chunku (0, 0) -> globální X = 15
    setWorldBlock(15, 0, 0, BlockType::Dirt);
    // Blok na levém kraji Chunku (1, 0) -> globální X = 16
    setWorldBlock(16, 0, 0, BlockType::Dirt); 

    // Vygenerujeme mesh POUZE pro první chunk
    const Chunk& chunk0 = world.getChunk(0, 0);
    ChunkMesher::generateMesh(world, chunk0, 0, 0, mesh);

    // Kdyby mesher neviděl do vedlejšího chunku, vygeneroval by 6 stěn (24 vrcholů).
    // Protože tam ale je blok z vedlejšího chunku, pravá stěna (+X) musí zmizet!
    // Očekáváme 5 stěn -> 20 vrcholů.
    EXPECT_EQ(mesh.vertices.size(), 20); 
}

TEST_F(ChunkMesherTest, AmbientOcclusionAtChunkBorder) {
    // Tráva na kraji Chunku (0,0) -> globální X = 15
    setWorldBlock(15, 0, 0, BlockType::Grass);
    // Zastínění způsobené vyvýšeným blokem ve vedlejším chunku -> globální X = 16
    setWorldBlock(16, 1, 0, BlockType::Dirt); 

    const Chunk& chunk0 = world.getChunk(0, 0);
    ChunkMesher::generateMesh(world, chunk0, 0, 0, mesh);

    ASSERT_FALSE(mesh.vertices.empty());

    bool shadowFound = false;
    for (const auto& vertex : mesh.vertices) {
        // Pokud je to horní stěna (normála míří nahoru) a leží na okraji u vedlejšího bloku (X=16)
        if (vertex.ny == 1.0f && vertex.x == 16.0f) { 
            // AO hodnota musí být menší než 1.0f (plné světlo), protože blok na X=16 stíní
            if (vertex.ao < 1.0f) {
                shadowFound = true;
                break;
            }
        }
    }
    
    EXPECT_TRUE(shadowFound) << "Ambient Occlusion nefunguje pres hranice chunku!";
}
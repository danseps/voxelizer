/**
 * @file worldGenerator.hpp
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2026-07-18
 * 
 * @copyright Copyright (c) 2026
 * 
 */
#pragma once

#include "world.hpp"
#include "chunkMesher.hpp"

/**
 * @brief Generates a single chunk with the specified offset
 * 
 * @param offsetX the X position of the chunk in a world (2D array)
 * @param offsetZ the Z position of the chunk in a world
 * 
 */
void generateChunk(Chunk& chunk, int offsetX, int offsetZ);

/**
 * @brief Generates the entire world with multiple chunks
 * 
 */
void generateWorld(World& world); //TODO: camera position,
// plnit world podle ní, generovat mesh z kazdeho chunku (z chunks mapy)

void generateWorldMesh(World& world, ChunkMesher::MeshData& mesh);
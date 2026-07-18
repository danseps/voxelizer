/**
 * @file perlinNoise.hpp
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2026-07-15
 * 
 * @copyright Copyright (c) 2026
 * 
 */

#pragma once

#include <glm/glm.hpp>

  /**
  * @brief Compute the dot product of the distance and gradient vectors.
  * 
  * @param ix 
  * @param iy 
  * @param x 
  * @param y 
  * @return float 
  */
float dotGridGradient(int ix, int iy, float x, float y);

/**
 * @brief Cubic interpolation function to smooth the transition between values
 * 
 * @param a0 
 * @param a1 
 * @param w 
 * @return float 
 */
float interpolate(float a0, float a1, float w);

/**
 * @brief Hash function to generate a pseudo-random gradient vector based on grid coordinates.
 * 
 * @param ix 
 * @param iy 
 * @return glm::vec2 
 */
glm::vec2 randomGradient(int ix, int iy);

 /**
  * @brief Sample the Perlin noise at coordinates (x, y).
  * 
  * @param x 
  * @param y 
  * @return float 
  */
 float perlin(float x, float y);

  /**
  * @brief Sample the octave-perlin noise at coordinates (x, y).
  * 
  * @param x 
  * @param y 
  * @param octaves 
  * @param persistence 
  * @return float 
  */
 float octavePerlin(float x, float y, int octaves, float persistence);
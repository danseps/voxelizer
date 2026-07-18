/**
 * @file perlinNoise.cpp
 * @author your name (you@domain.com)
 * @brief 
 * @see https://youtu.be/kCIaHqb60Cw?si=ITMiZR151Z42EbFt
 * @version 0.1
 * @date 2026-07-15
 * 
 * @copyright Copyright (c) 2026
 * 
 */

#include "perlinNoise.hpp"

float dotGridGradient(int ix, int iy, float x, float y)
{
    // Get the gradient from the integer coordinates
    glm::vec2 gradient = randomGradient(ix, iy);

    // Compute the distance vector
    float dx = x - static_cast<float>(ix);
    float dy = y - static_cast<float>(iy);

    // Compute the dot-product
    return (dx * gradient.x + dy * gradient.y);
}

float interpolate(float a0, float a1, float w)
{
    // Use smoothstep interpolation
    return (a1 - a0) * (3.0f - w * 2.0f) * w * w + a0;
}

glm::vec2 randomGradient(int ix, int iy)
{
    // No precomputed gradients mean this works for any number of grid coordinates
    const unsigned w = 8 * sizeof(unsigned);
    const unsigned s = w / 2; // rotation width
    unsigned a = ix, b = iy;

    a *= 3284157443u; b ^= a << s | a >> (w - s);
    b *= 1911520717u; a ^= b << s | b >> (w - s);
    a *= 2048419325u;

    float random = a * (3.14159265 / ~(~0u >>1)); // in [0, 2*Pi]

    return glm::vec2(sin(random), cos(random));
}

 float perlin(float x, float y)
 {
    // Determine the grid cell coordinates
    int x0 = static_cast<int>(std::floor(x));
    int y0 = static_cast<int>(std::floor(y));
    int x1 = x0 + 1;
    int y1 = y0 + 1;

    // Compute Interpolation weights
    float sx = x - static_cast<float>(x0);
    float sy = y - static_cast<float>(y0);

    // Compute and interpolate top two corners
    float n0 =  dotGridGradient(x0, y0, x, y);
    float n1 =  dotGridGradient(x1, y0, x, y);
    float ix0 = interpolate(n0, n1, sx);

    // Compute and interpolate bottom two corners
    n0 =  dotGridGradient(x0, y1, x, y);
    n1 =  dotGridGradient(x1, y1, x, y);
    float ix1 = interpolate(n0, n1, sx);

    // Interpolate between the two interpolated values, now in y
    return interpolate(ix0, ix1, sy);
 }

 float octavePerlin(float x, float y, int octaves, float persistence)
 {
    float total = 0.0f;
    float frequency = 1.0f;
    float amplitude = 1.0f;
    float maxValue = 0.0f; // Used for normalizing result to [0, 1]

    for (int i = 0; i < octaves; i++)
    {
        total += perlin(x * frequency, y * frequency) * amplitude;

        maxValue += amplitude;

        amplitude *= persistence;
        frequency *= 2.0f;
    }

    return total / maxValue;
 }
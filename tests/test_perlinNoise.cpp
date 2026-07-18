/**
 * @file test_perlinNoise.cpp
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2026-07-18
 * 
 * @copyright Copyright (c) 2026
 * 
 */

 #include <gtest/gtest.h>
#include "../src/perlinNoise.hpp"

TEST(PerlinTest, ValueBounds) {
    for (float x = 0.0f; x < 10.0f; x += 0.5f) {
        for (float y = 0.0f; y < 10.0f; y += 0.5f) {
            float val = octavePerlin(x, y, 6, 0.5f);
            EXPECT_GE(val, -1.0f);
            EXPECT_LE(val, 1.0f);
        }
    }
}

TEST(PerlinTest, DeterministicOutput) {
    float x = 10.5f;
    float y = 20.5f;
    float firstCall = perlin(x, y);
    float secondCall = perlin(x, y);
    EXPECT_FLOAT_EQ(firstCall, secondCall);
}
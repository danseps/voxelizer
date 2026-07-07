/**
 * @file utils.hpp
 * @author Daniel Šeps (daniel.seps@proton.me)
 * @brief Utility functions and structures
 * @version 0.1
 * @date 2026-04-08
 * 
 * @copyright Copyright (c) 2026
 * 
 */
#ifndef UTILS_HPP
#define UTILS_HPP
#pragma once

namespace utils
{
    /** @brief Represents a color with RGBA components for OpenGL */
    struct Color {
        float r, g, b, a;
    };

    /**
     * @brief Create a Color object
     * 
     * @param r red component (0-255)
     * @param g green component (0-255)
     * @param b blue component (0-255)
     * @param a transparency (0.0-1.0), default is 1.0 (fully opaque)
     * @return Color 
     */
    inline Color createColor(float r, float g, float b, float a = 1.0f) {
        return {r/255.0f, g/255.0f, b/255.0f, a};
    }

    // define some basic colors for convenience
    const Color RED = createColor(255.0f, 0.0f, 0.0f);
    const Color GREEN = createColor(0.0f, 255.0f, 0.0f);
    const Color BLUE = createColor(0.0f, 0.0f, 255.0f);
    const Color WHITE = createColor(255.0f, 255.0f, 255.0f);
    const Color BLACK = createColor(0.0f, 0.0f, 0.0f);
    const Color YELLOW = createColor(255.0f, 255.0f, 0.0f);
    const Color CYAN = createColor(0.0f, 255.0f, 255.0f);
    const Color MAGENTA = createColor(255.0f, 0.0f, 255.0f);
    const Color ORANGE = createColor(255.0f, 165.0f, 0.0f);
    const Color PURPLE = createColor(128.0f, 0.0f, 128.0f);
    const Color PINK = createColor(255.0f, 192.0f, 203.0f);
    const Color LIME = createColor(128.0f, 255.0f, 0.0f);
    const Color GOLD = createColor(255.0f, 215.0f, 0.0f);
    const Color BROWN = createColor(165.0f, 42.0f, 42.0f);
    const Color GRAY = createColor(128.0f, 128.0f, 128.0f);
}

#endif // UTILS_HPP
/**
 * @file Color.h
 * @author Dorian LEXTERIAQUE (dlexteriaque@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2025-06-28
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#ifndef COLOR_H
#define COLOR_H

#include <cmath>
#include <iostream>
#include <stdexcept>

#include "Common/dllExport.h"

namespace PulseEngine
{
    /**
     * @brief Color class represents a color in RGBA format.
     * 
     */
    struct PULSE_ENGINE_DLL_API Color
    {
        float r;
        float g;
        float b;
        float a;
        /**
         * @brief Default constructor initializes color to black (0, 0, 0) with full opacity (1).
         */
        Color() : r(0), g(0), b(0), a(1) {}
        /**
         * @brief Constructor that initializes color with given red, green, blue, and alpha values.
         * 
         * @param scalar The scalar value to set for r, g, b channels (a will be set to 1).
         */
        Color(float scalar) : r(scalar), g(scalar), b(scalar), a(1) {}
        /**
         * @brief Constructor that initializes color with given red, green, blue, and alpha values.
         * 
         * @param r Red channel value.
         * @param g Green channel value.
         * @param b Blue channel value.
         * @param a Alpha channel value (default is 1).
         */
        Color(float r, float g, float b, float a = 1) : r(r), g(g), b(b), a(a) {}
        /**
         * @brief Constructor that initializes color with given red, green, and blue values, and sets alpha to 1.
         * 
         * @param r Red channel value.
         * @param g Green channel value.
         * @param b Blue channel value.
         */
        Color(const Color& color) : r(color.r), g(color.g), b(color.b), a(color.a) {}
        Color& operator=(const Color& color)
        {
            if (this != &color)
            {
                r = color.r;
                g = color.g;
                b = color.b;
                a = color.a;
            }
            return *this;
        }
    };
}

#endif

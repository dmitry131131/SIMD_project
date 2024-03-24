#include <stdio.h>
#include <assert.h>
#include <SFML/Graphics.hpp>

#include "simd.h"

void generate_image_by_pixel(sf::Uint32* array)
{
    assert(array);

    for (size_t line = 0; line < WINDOW_HEIGHT; line++)
    {
        float x_0 = (-((float) WINDOW_WIDTH / 2) * dx + X_offset) * scale;
        float y_0 = (((float) line - ((float) WINDOW_HEIGHT / 2)) * dy) * scale;
        
        for (size_t col = 0; col < WINDOW_WIDTH; col++, x_0 += dx*scale)
        {
            float x_n = x_0;
            float y_n = y_0;
            size_t count = 0;

            while (count < MAX_ITERATIONS)
            {
                count++;
                float X2 = x_n*x_n;
                float Y2 = y_n*y_n;
                float XY = x_n*y_n;

                float len = X2 + Y2;

                if (len >= RADIUS2) break;

                x_n = X2 - Y2 + x_0;
                y_n = XY + XY + y_0;
            }
            array[line*WINDOW_WIDTH + col] = (sf::Uint32) (0xffffffff - 180*count);   // Магическое число в определении цвета
        } 
    }
}

void generate_image_by_line(sf::Uint32* array)
{
    assert(array);

    float real_dx = dx*scale;

    for (size_t line = 0; line < WINDOW_HEIGHT; line++)
    {
        float x_0 = (-((float) WINDOW_WIDTH / 2) * dx + X_offset) * scale;
        float y_0 = (((float) line - ((float) WINDOW_HEIGHT / 2)) * dy) * scale;
        
        for (size_t col = 0; col < WINDOW_WIDTH; col += 8, x_0 += 8*real_dx)
        {
            float X0[8] = {x_0, x_0 + real_dx, x_0 + 2*real_dx, x_0 + 3*real_dx, x_0 + 4*real_dx, x_0 + 5*real_dx, x_0 + 6*real_dx, x_0 + 7*real_dx};

            float X_N[8] = {}; for (size_t i = 0; i < 8; i++) X_N[i] = X0[i];
            float Y_N[8] = {}; for (size_t i = 0; i < 8; i++) Y_N[i] = y_0;
            int count = 0;
            int real_count[8] = {};

            while (count < MAX_ITERATIONS)
            {
                count++;
                float X2[8] = {}; for (size_t i = 0; i < 8; i++) X2[i] = X_N[i] * X_N[i];
                float Y2[8] = {}; for (size_t i = 0; i < 8; i++) Y2[i] = Y_N[i] * Y_N[i];
                float XY[8] = {}; for (size_t i = 0; i < 8; i++) XY[i] = X_N[i] * Y_N[i];

                float R2[8] = {}; for (size_t i = 0; i < 8; i++) R2[i] = X2[i] + Y2[i];

                int cmp[8] = {};
                for (size_t i = 0; i < 8; i++) if (R2[i] <= RADIUS2) cmp[i] = 1;

                int mask = 0;
                for (size_t i = 0; i < 8; i++) mask |= (cmp[i] << i);
                if (!mask) break;

                for (size_t i = 0; i < 8; i++) real_count[i] += cmp[i];

                for (size_t i = 0; i < 8; i++) X_N[i] = X2[i] - Y2[i] + X0[i];
                for (size_t i = 0; i < 8; i++) Y_N[i] = XY[i] + XY[i] + y_0;
            }
            for (size_t i = 0; i < 8; i++) array[line*WINDOW_WIDTH + col + i] = (sf::Uint32) (0xffffffff - 180 * real_count[i]);
        } 
    }
}
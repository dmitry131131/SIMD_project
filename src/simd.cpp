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

    for (size_t line = 0; line < WINDOW_HEIGHT; line++)
    {
        float x_0 = (-((float) WINDOW_WIDTH / 2) * dx + X_offset) * scale;
        float y_0 = (((float) line - ((float) WINDOW_HEIGHT / 2)) * dy) * scale;

        size_t array_offset = line * WINDOW_WIDTH;
        
        for (size_t col = 0; col < WINDOW_WIDTH; col += 4, x_0 += 4*dx*scale)
        {
            float x_n[4] = {x_0, x_0 + dx, x_0 + (2*dx), x_0 + (3*dx)};
            float y_n[4] = {y_0, y_0, y_0, y_0};
            size_t count = 0;

            size_t final_counts[4] = {};

            int if_count = 4;

            while (count < MAX_ITERATIONS)
            {
                count++;
                float X2[4]  = {}; for (size_t i = 0; i < 4; i++) X2[i] = x_n[i] * x_n[i];
                float Y2[4]  = {}; for (size_t i = 0; i < 4; i++) Y2[i] = y_n[i] * y_n[i];
                float XY[4]  = {}; for (size_t i = 0; i < 4; i++) XY[i] = x_n[i] * y_n[i];

                float len[4] = {}; for (size_t i = 0; i < 4; i++) len[i] = X2[i] * Y2[i];

                for (size_t i = 0; i < 4; i++) 
                    if (len[i] > RADIUS2) 
                    {
                        if (!final_counts[i]) 
                        {
                            final_counts[i] = count;
                            if_count--;
                        }
                    }

                if (!if_count) break;

                for (size_t i = 0; i < 4; i++) x_n[i] = X2[i] - Y2[i] + x_0 + dx*((float) i);
                for (size_t i = 0; i < 4; i++) y_n[i] = 2*XY[i] + y_0;
            }
            for (size_t i = 0; i < 4; i++)
            {
                array[array_offset + col + i] = (sf::Uint32) (0xffffffff - 180*final_counts[i]);
            }
        } 
    }
}
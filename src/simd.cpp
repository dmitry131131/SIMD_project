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
                array[line*WINDOW_WIDTH + col] = (sf::Uint32) (0xffffffff / (MAX_ITERATIONS + 1 - count));
        } 
    }
}
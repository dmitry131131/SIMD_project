#include <stdio.h>
#include <SFML/Graphics.hpp>

#include "simd.h"

void generate_image(sf::Uint32 array[WINDOW_HEIGHT][WINDOW_WIDTH])
{
    for (size_t line = 0; line < WINDOW_HEIGHT; line++)
    {
        float x_0 = (-(400.f)*dx - 0.2) * scale;
        float y_0 = (((float) line - (300.f))*dy) * scale;
        
        for (size_t col = 0; col < WINDOW_WIDTH; col++, x_0 += dx*scale)
        {
            float x_n = x_0;
            float y_n = y_0;
            size_t count = 0;

            while (count < 256)
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

            if (count == 256)
            {
                array[line][col] = 0xffffffff;
            }
        } 
    }
}
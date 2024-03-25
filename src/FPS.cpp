#include <stdio.h>
#include <SFML/Graphics.hpp>
#include "FPS.h"

void calculate_FPS(FPS_data* FPS, sf::Text* text)
{
    clock_t current_time = clock() / CLOCKS_PER_SEC;
    char FPS_buffer[16] = {};
    if ((current_time - FPS->last_time) > 0.5)
    {
        sprintf(FPS_buffer, "%lu", FPS->frame_counter);
        (*text).setString(FPS_buffer);
        FPS->last_time     = current_time;
        FPS->frame_counter = 0;
    }
}
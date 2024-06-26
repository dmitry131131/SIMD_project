#include <stdio.h>
#include <SFML/Graphics.hpp>

#include "simd.h"
#include "FPS.h"

void calculate_FPS(FPS_data* FPS, render_context* context)
{
    const char* mode_name[3] = {"Pixel", "Line", "SIMD"};
    char FPS_buffer[32]      = {};

    clock_t current_time = clock() / CLOCKS_PER_SEC;
    if ((current_time - FPS->last_time) > 0.5)
    {
        sprintf(FPS_buffer, "%lu mode: %s(%lu)", FPS->frame_counter, mode_name[context->mode], (context->mode)+1);
        (context->text).setString(FPS_buffer);
        FPS->last_time     = current_time;
        FPS->frame_counter = 0;
    }
}

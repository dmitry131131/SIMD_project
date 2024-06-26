#ifndef FPS_H
#define FPS_H

#include <time.h>

struct FPS_data {
    clock_t last_time;
    size_t frame_counter;
};

void calculate_FPS(FPS_data* FPS, render_context* context);

#endif
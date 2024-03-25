#ifndef SIMD_H
#define SIMD_H

const float RADIUS2 = 100;
const size_t MAX_ITERATIONS = 256;
const size_t WINDOW_WIDTH   = 800;
const size_t WINDOW_HEIGHT  = 600;

const float dx = 1 / (float) WINDOW_WIDTH;
const float dy = 1 / (float) WINDOW_WIDTH;

enum render_mode_t {
    PIXEL = 0,
    LINE  = 1,
    SIMD  = 2
};

void generate_image_by_pixel(sf::Uint32* array, const float X_offset, const float Y_offset, const float scale, const int color_constant);

void generate_image_by_line(sf::Uint32* array, const float X_offset, const float Y_offset, const float scale, const int color_constant);

void generate_image_by_simd(sf::Uint32* array, const float X_offset, const float Y_offset, const float scale, const int color_constant);

void compare_mode(sf::Uint32* frame, const float X_offset, const float Y_offset, const float scale, const int color_constant);

#endif
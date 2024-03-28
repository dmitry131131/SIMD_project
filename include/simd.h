#ifndef SIMD_H
#define SIMD_H

const float  RADIUS2        = 100;
const size_t MAX_ITERATIONS = 256;
const size_t WINDOW_WIDTH   = 800;
const size_t WINDOW_HEIGHT  = 600;
const int    color_constant = 180;

const float dx = 1 / (float) WINDOW_WIDTH;
const float dy = 1 / (float) WINDOW_WIDTH;

enum render_mode_t {
    PIXEL = 0,
    LINE  = 1,
    SIMD  = 2
};

struct render_context {
    sf::Font font;
    sf::Text text;
    sf::Uint32* frame;
    render_mode_t mode;
    float scale; 
    float X_offset; 
    float Y_offset;
};

void generate_image_by_pixel(render_context* context);
void generate_image_by_line( render_context* context);
void generate_image_by_simd( render_context* context);
void compare_mode(           render_context* context);

void context_ctor(render_context* context);
void context_dtor(render_context* context);

#endif
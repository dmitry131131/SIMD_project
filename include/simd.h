#ifndef SIMD_H
#define SIMD_H

const float RADIUS2 = 100;
const size_t MAX_ITERATIONS = 256;
const size_t WINDOW_WIDTH   = 800;
const size_t WINDOW_HEIGHT  = 600;

const float dx = 1 / (float) WINDOW_WIDTH;
const float dy = 1 / (float) WINDOW_WIDTH;

const float scale = 2.9f, X_offset = -0.25f;

void generate_image_by_pixel(sf::Uint32* array);

#endif
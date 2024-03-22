#ifndef SIMD_H
#define SIMD_H

const float RADIUS2 = 100;
const size_t WINDOW_WIDTH  = 800;
const size_t WINDOW_HEIGHT = 600;

const float dx = 1/800.f;
const float dy = 1/600.f;

const float scale = 2.5f;

void generate_image(sf::Uint32 array[WINDOW_HEIGHT][WINDOW_WIDTH]);

#endif
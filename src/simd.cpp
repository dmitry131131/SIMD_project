#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <immintrin.h>
#include <string.h>
#include <SFML/Graphics.hpp>

#include "simd.h"

void generate_image_by_pixel(render_context* context)
{
    assert(context);

    for (size_t line = 0; line < WINDOW_HEIGHT; line++)
    {
        float x_0 = (-((float) WINDOW_WIDTH / 2) * dx + context->X_offset) * context->scale;
        float y_0 = (((float) line - ((float) WINDOW_HEIGHT / 2)) * dy + context->Y_offset) * context->scale;
        
        for (size_t col = 0; col < WINDOW_WIDTH; col++, x_0 += dx*context->scale)
        {
            float x_n    = x_0;
            float y_n    = y_0;
            size_t count = 0;

            while (count <= MAX_ITERATIONS)
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
            (context->frame)[line*WINDOW_WIDTH + col] = (sf::Uint32) (0xffffffff - color_constant * (count - 1));   // Магическое число в определении цвета
        } 
    }
}

void generate_image_by_line(render_context* context)
{
    assert(context);

    float real_dx = dx*context->scale;

    for (size_t line = 0; line < WINDOW_HEIGHT; line++)
    {
        float x_0 = (-((float) WINDOW_WIDTH / 2) * dx + context->X_offset) * context->scale;
        float y_0 = (((float) line - ((float) WINDOW_HEIGHT / 2)) * dy + context->Y_offset) * context->scale;
        
        for (size_t col = 0; col < WINDOW_WIDTH; col += 8, x_0 += 8*real_dx)
        {
            float X0[8] = {x_0, x_0 + real_dx, x_0 + 2*real_dx, x_0 + 3*real_dx, x_0 + 4*real_dx, x_0 + 5*real_dx, x_0 + 6*real_dx, x_0 + 7*real_dx};

            float X_N[8]      = {}; for (size_t i = 0; i < 8; i++) X_N[i] = X0[i];
            float Y_N[8]      = {}; for (size_t i = 0; i < 8; i++) Y_N[i] = y_0;
            int count         = 0;
            int real_count[8] = {};

            while (count < MAX_ITERATIONS)
            {
                count++;
                float X2[8] = {}; for (size_t i = 0; i < 8; i++) X2[i] = X_N[i] * X_N[i];
                float Y2[8] = {}; for (size_t i = 0; i < 8; i++) Y2[i] = Y_N[i] * Y_N[i];
                float XY[8] = {}; for (size_t i = 0; i < 8; i++) XY[i] = X_N[i] * Y_N[i];

                float R2[8] = {}; for (size_t i = 0; i < 8; i++) R2[i] = X2[i] + Y2[i];

                int cmp[8]  = {};
                for (size_t i = 0; i < 8; i++) if (R2[i] <= RADIUS2) cmp[i] = 1;

                int mask = 0;
                for (size_t i = 0; i < 8; i++) mask |= (cmp[i] << i);
                if (!mask) break;

                for (size_t i = 0; i < 8; i++) real_count[i] += cmp[i];

                for (size_t i = 0; i < 8; i++) X_N[i] = X2[i] - Y2[i] + X0[i];
                for (size_t i = 0; i < 8; i++) Y_N[i] = XY[i] + XY[i] + y_0;
            }
            for (size_t i = 0; i < 8; i++) (context->frame)[line*WINDOW_WIDTH + col + i] = (sf::Uint32) (0xffffffff - color_constant * real_count[i]);
        } 
    }
}

void generate_image_by_simd(render_context* context)
{
   assert(context);

    float real_dx    = dx*context->scale;
    __m256 MaxRadius = _mm256_set1_ps(RADIUS2);

    for (size_t line = 0; line < WINDOW_HEIGHT; line++)
    {
        float x_0 = (-((float) WINDOW_WIDTH / 2) * dx + context->X_offset) * context->scale;
        float y_0 = (((float) line - ((float) WINDOW_HEIGHT / 2)) * dy + context->Y_offset) * context->scale;
        
        for (size_t col = 0; col < WINDOW_WIDTH; col += 8, x_0 += 8*real_dx)
        {
            __m256 DX = _mm256_set1_ps(real_dx);
            __m256 Mul_array = _mm256_set_ps(7, 6, 5, 4, 3, 2, 1, 0);
            DX = _mm256_mul_ps(DX, Mul_array);
            __m256 X0 = _mm256_set1_ps(x_0);
            X0 = _mm256_add_ps(X0, DX);

            __m256 Y0  = _mm256_set1_ps(y_0);

            __m256 X_N = X0;
            __m256 Y_N = Y0;

            int count = 0;
            __m256i real_count = _mm256_setzero_si256();
            while (count < MAX_ITERATIONS)
            {
                count++;
                __m256 X2 = _mm256_mul_ps(X_N, X_N);
                __m256 Y2 = _mm256_mul_ps(Y_N, Y_N);
                __m256 XY = _mm256_mul_ps(X_N, Y_N);

                __m256 R2 = _mm256_add_ps(X2, Y2);

                __m256 res = _mm256_cmp_ps(R2, MaxRadius, _CMP_LE_OS);

                if (!_mm256_movemask_ps(res)) break;

                __m256i temp = _mm256_castps_si256(res);
                temp = _mm256_srli_epi32(temp, 31);
                real_count = _mm256_add_epi32(real_count, temp);

                X_N = _mm256_sub_ps(X2, Y2);
                X_N = _mm256_add_ps(X_N, X0);

                Y_N = _mm256_add_ps(XY, XY);
                Y_N = _mm256_add_ps(Y_N, Y0);
            }
            __m256i out_array  = _mm256_set1_epi32(0xffffffff);
            __m256i temp_color = _mm256_set1_epi32(color_constant);
            temp_color         = _mm256_mullo_epi32(temp_color, real_count);
            out_array          = _mm256_sub_epi32(out_array, temp_color);

            memcpy(context->frame + line*WINDOW_WIDTH + col, &out_array, 32);
        } 
    }
}

void compare_mode(render_context* context)
{
    size_t start_time = __rdtsc();

    for (size_t i = 0; i < 20; i++)
    {
        generate_image_by_pixel(context);
    }

    size_t end_time = __rdtsc();

    printf("pixel time: %ld\n", end_time - start_time);

    start_time = __rdtsc();

    for (size_t i = 0; i < 20; i++)
    {
        generate_image_by_line(context);
    }

    end_time = __rdtsc();

    printf("line time:  %ld\n", end_time - start_time);

    start_time = __rdtsc();

    for (size_t i = 0; i < 20; i++)
    {
        generate_image_by_simd(context);
    }

    end_time = __rdtsc();

    printf("simd time:  %ld\n", end_time - start_time);
}

void context_ctor(render_context* context)
{
    assert(context);

    context->frame    = (sf::Uint32*) calloc(WINDOW_HEIGHT * WINDOW_WIDTH, sizeof(sf::Uint32));
    context->X_offset = -0.25f;
    context->Y_offset = 0;
    context->scale    = 2.9f;

    (context->font).loadFromFile("font/font.ttf");
    (context->text).setFont(context->font);
    (context->text).setCharacterSize(22);
    (context->text).setFillColor(sf::Color::Red);
    (context->text).setStyle(sf::Text::Regular);
    
    context->mode = PIXEL;
}

void context_dtor(render_context* context)
{
    assert(context);

    free(context->frame);
}
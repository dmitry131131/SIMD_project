#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <immintrin.h>
#include <SFML/Graphics.hpp>

#include "image.h"

ImageData open_image(const char* file_name)
{
    assert(file_name);

    sf::Image src_img;
    src_img.loadFromFile(file_name);
    const sf::Uint8 *pixel_array = src_img.getPixelsPtr();
    sf::Vector2u len_vec         = src_img.getSize();
    size_t img_len               = len_vec.x * len_vec.y;        // Вычисляем кол-во пикселей

    sf::Uint8* dst_array = (sf::Uint8*) calloc(img_len + 1, sizeof(sf::Uint8)*4);   // На каждый пиксель по 4 ячейки 

    memcpy(dst_array, pixel_array, img_len*4);  // На каждый пиксель по 4 ячейки 

    ImageData data = {.name = dst_array, .size = len_vec, .pixel_count = img_len};

    return data;
}

sf::Texture get_texture(ImageData* image)
{
    assert(image);

    sf::Texture texture;
    texture.create(image->size.x, image->size.y);
    texture.update((sf::Uint8*) image->name, image->size.x, image->size.y, 0, 0);

    return texture;
}

ImageData img_data_cpy(ImageData* src)
{
    assert(src);

    ImageData dst = {};

    dst.pixel_count = src->pixel_count;
    dst.size        = src->size;
    dst.name        = (sf::Uint8*) calloc(src->pixel_count + 1, sizeof(sf::Uint8)*4);

    memcpy(dst.name, src->name, (src->pixel_count * 4));

    return dst;
}

void make_composing_pixel(const ImageData* first, const ImageData* second, ImageData* out)
{
    assert(first);
    assert(second);
    assert(out);

    size_t lines = first->size.y;
    size_t cols  = first->size.x;

    for (size_t i = 0; i < lines; i++)
    {
        for (size_t j = 0; j < cols * 4; j++)
        {
            size_t current_position = i * cols * 4 + j; 
            
            float table = first->name[current_position]/255.f;
            float cat   = second->name[current_position]/255.f;

            table *= cat;

            out->name[current_position]   = (sf::Uint8) (table * 255.f);
        }
    }
}

void make_composing_line(const ImageData* first,const ImageData* second, ImageData* out)
{
    assert(first);
    assert(second);
    assert(out);

    size_t lines = first->size.y;
    size_t cols  = first->size.x;

    for (size_t i = 0; i < lines; i++)
    {
        for (size_t j = 0; j < cols * 4; j += 32)
        {
            size_t current_position = i * cols * 4 + j; 

            float out_array[32]    = {}; for (size_t k = 0; k < 32; k++) {out_array[k]    = first->name[current_position + k]/255.f;};
            float second_array[32] = {}; for (size_t k = 0; k < 32; k++) {second_array[k] = second->name [current_position + k]/255.f;};

            for (size_t k = 0; k < 32; k++) {out_array[k] *= second_array[k];};

            for (size_t k = 0; k < 32; k++) {out->name[current_position + k] = (sf::Uint8) (out_array[k] * 255.f);}
        }
    }
}

void make_composing_simd(const ImageData* first, const ImageData* second, ImageData* out)
{
    assert(first);
    assert(second);
    assert(out);

    size_t lines = first->size.y;
    size_t cols  = first->size.x;

    for (size_t i = 0; i < lines; i++)
    {
        for (size_t j = 0; j < cols * 4; j += 8)
        {
            size_t current_position = i * cols * 4 + j;

            __m256i out_dec_array    = _mm256_set_epi32((first->name[current_position + 0]), (first->name[current_position + 1]),
                                                        (first->name[current_position + 2]), (first->name[current_position + 3]),
                                                        (first->name[current_position + 4]), (first->name[current_position + 5]),
                                                        (first->name[current_position + 6]), (first->name[current_position + 7]));

            __m256i second_dec_array = _mm256_set_epi32((second->name[current_position + 0]), (second->name[current_position + 1]),
                                                        (second->name[current_position + 2]), (second->name[current_position + 3]),
                                                        (second->name[current_position + 4]), (second->name[current_position + 5]),
                                                        (second->name[current_position + 6]), (second->name[current_position + 7]));

            __m256 out_array    = _mm256_cvtepi32_ps(out_dec_array);
            __m256 second_array = _mm256_cvtepi32_ps(second_dec_array);

            __m256 mul = _mm256_set1_ps(255.f);

            out_array    = _mm256_div_ps(out_array, mul);
            second_array = _mm256_div_ps(second_array, mul);

            out_array = _mm256_mul_ps(out_array, second_array);
            out_array = _mm256_mul_ps(out_array, mul);

            __m256i temp = _mm256_cvtps_epi32(out_array);

            for (size_t k = 0; k < 8; k++)
            {
                out->name[current_position + k] = (sf::Uint8) _mm256_extract_epi32(temp, 7-k);
            }
        }
    }
}

void make_alpha_composing_pixel(const ImageData* first, const ImageData* second, ImageData* out)
{
    assert(first);
    assert(second);
    assert(out);

    size_t lines = first->size.y;
    size_t cols  = first->size.x;

    for (size_t i = 0; i < lines*cols; i++)
    {
        size_t current_position = i * 4; 

        float out_alpha    = first->name [current_position + 3]/255.f;
        float second_alpha = second->name[current_position + 3]/255.f;

        float R_A = out_alpha + second_alpha * (1 - out_alpha);

        for (size_t j = 0; j < 4; j++)
        {
            float out_chanel    = first->name[current_position + j]/255.f;
            float second_chanel = second->name [current_position + j]/255.f;

            out_chanel = (second_chanel * second_alpha + out_chanel * out_alpha * (1 - second_alpha))/R_A;

            out->name[current_position + j] = (sf::Uint8) (out_chanel * 255.f);
        }
    }
}

void make_alpha_composing_line(const ImageData* first,const ImageData* second, ImageData* out)
{
    assert(first);
    assert(second);
    assert(out);

    size_t lines = first->size.y;
    size_t cols  = first->size.x;

    for (size_t i = 0; i < lines; i++)
    {
        for (size_t j = 0; j < cols * 4; j += 4)
        {
            size_t current_position = i * cols * 4 + j; 

            float out_array[4]    = {}; for (size_t k = 0; k < 4; k++) {out_array[k]    = first->name[current_position + k]/255.f;};
            float second_array[4] = {}; for (size_t k = 0; k < 4; k++) {second_array[k] = second->name [current_position + k]/255.f;};

            float R_A = out_array[3] + second_array[3]*(1-out_array[3]);

            for (size_t k = 0; k < 4; k++) {out_array[k] = (second_array[k]*second_array[3] + out_array[k]*out_array[3]*(1-second_array[3]))/R_A;}

            for (size_t k = 0; k < 4; k++) {out->name[current_position + k] = (sf::Uint8) (out_array[k] * 255.f);}
        }
    }
}

void make_alpha_composing_simd(const ImageData* first, const ImageData* second, ImageData* out)
{
    assert(first);
    assert(second);
    assert(out);

    size_t lines = first->size.y;
    size_t cols  = first->size.x;

    for (size_t i = 0; i < lines; i++)
    {
        for (size_t j = 0; j < cols * 4; j += 8)
        {
            size_t current_position = i * cols * 4 + j;

            __m256i out_dec_array    = _mm256_set_epi32((first->name[current_position + 0]), (first->name[current_position + 1]),
                                                        (first->name[current_position + 2]), (first->name[current_position + 3]),
                                                        (first->name[current_position + 4]), (first->name[current_position + 5]),
                                                        (first->name[current_position + 6]), (first->name[current_position + 7]));
            __m256i out_dec_alpha    = _mm256_set_epi32((first->name[current_position + 3]), (first->name[current_position + 3]),
                                                        (first->name[current_position + 3]), (first->name[current_position + 3]),
                                                        (first->name[current_position + 7]), (first->name[current_position + 7]),
                                                        (first->name[current_position + 7]), (first->name[current_position + 7]));

            __m256i second_dec_array = _mm256_set_epi32((second->name[current_position + 0]), (second->name[current_position + 1]),
                                                        (second->name[current_position + 2]), (second->name[current_position + 3]),
                                                        (second->name[current_position + 4]), (second->name[current_position + 5]),
                                                        (second->name[current_position + 6]), (second->name[current_position + 7]));
            __m256i second_dec_alpha = _mm256_set_epi32((second->name[current_position + 3]), (second->name[current_position + 3]),
                                                        (second->name[current_position + 3]), (second->name[current_position + 3]),
                                                        (second->name[current_position + 7]), (second->name[current_position + 7]),
                                                        (second->name[current_position + 7]), (second->name[current_position + 7]));

            __m256 out_array          = _mm256_cvtepi32_ps(out_dec_array);
            __m256 second_array       = _mm256_cvtepi32_ps(second_dec_array);
            __m256 out_array_alpha    = _mm256_cvtepi32_ps(out_dec_alpha);
            __m256 second_array_alpha = _mm256_cvtepi32_ps(second_dec_alpha);

            __m256 mul = _mm256_set1_ps(255.f);

            out_array          = _mm256_div_ps(out_array, mul);
            second_array       = _mm256_div_ps(second_array, mul);
            out_array_alpha    = _mm256_div_ps(out_array_alpha, mul);
            second_array_alpha = _mm256_div_ps(second_array_alpha, mul);

            __m256 R_A = _mm256_set1_ps(1.f);
            R_A        = _mm256_sub_ps(R_A, out_array_alpha);
            R_A        = _mm256_mul_ps(R_A, second_array_alpha);
            R_A        = _mm256_add_ps(R_A, out_array_alpha);


            __m256 temp_array = _mm256_set1_ps(1.f);

            temp_array = _mm256_sub_ps(temp_array, second_array_alpha);
            temp_array = _mm256_mul_ps(temp_array, out_array_alpha);
            temp_array = _mm256_mul_ps(temp_array, out_array);

            out_array = _mm256_mul_ps(second_array, second_array_alpha);

            out_array = _mm256_add_ps(temp_array, out_array);

            out_array = _mm256_div_ps(out_array, R_A);
            
            out_array = _mm256_mul_ps(out_array, mul);

            __m256i temp = _mm256_cvtps_epi32(out_array);

            for (size_t k = 0; k < 8; k++)
            {
                out->name[current_position + k] = (sf::Uint8) _mm256_extract_epi32(temp, 7-k);
            }
        }
    }
}

void cat_compare_mode(ImageData* first, ImageData* second, ImageData* out)
{
    size_t start_time = __rdtsc();

    for (size_t i = 0; i < 100; i++)
    {
        #ifdef MULTIPLY_MODE
        make_composing_pixel(first, second, out);
        #else
        make_alpha_composing_pixel(first, second, out);
        #endif
    }

    size_t end_time = __rdtsc();

    printf("pixel time: %ld\n", end_time - start_time);

    start_time = __rdtsc();

    for (size_t i = 0; i < 100; i++)
    {
        #ifdef MULTIPLY_MODE
        make_composing_line(first, second, out);
        #else
        make_alpha_composing_line(first, second, out);
        #endif
    }

    end_time = __rdtsc();

    printf("line time:  %ld\n", end_time - start_time);

    start_time = __rdtsc();

    for (size_t i = 0; i < 100; i++)
    {
        #ifdef MULTIPLY_MODE
        make_composing_simd(first, second, out);
        #else
        make_alpha_composing_simd(first, second, out);
        #endif
    }

    end_time = __rdtsc();

    printf("simd time:  %ld\n", end_time - start_time);
}

void save_image(ImageData* image, const char* filename)
{
    assert(image);
    assert(filename);

    sf::Image temp_img;
    temp_img.create(image->size.x, image->size.y, image->name);
    temp_img.saveToFile(filename);
}
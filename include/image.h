#ifndef IMAGE_H
#define IMAGE_H

//#define MULTIPLY_MODE

struct ImageData {
    sf::Uint8*   name;
    sf::Vector2u size;
    size_t       pixel_count;
};

struct pix {
    float R;
    float G;
    float B;
    float A;
};

ImageData  open_image(const char* file_name);
sf::Texture get_texture(ImageData* image);

void make_composing_pixel(const ImageData* first, const ImageData* second, ImageData* out);
void make_composing_line(const ImageData* first,const ImageData* second, ImageData* out);
void make_composing_simd(const ImageData* first, const ImageData* second, ImageData* out);

void make_alpha_composing_pixel(const ImageData* first, const ImageData* second, ImageData* out);
void make_alpha_composing_line(const ImageData* first,const ImageData* second, ImageData* out);
void make_alpha_composing_simd(const ImageData* first, const ImageData* second, ImageData* out);

void cat_compare_mode(ImageData* first, ImageData* second, ImageData* out);

ImageData img_data_cpy(ImageData* src);
void save_image(ImageData* image, const char* filename);

#endif
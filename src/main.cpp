#include <stdio.h>
#include <time.h>
#include <SFML/Graphics.hpp>

#include "simd.h"

int main()
{

    sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Mandelbrot", sf::Style::Titlebar);

    sf::Uint32* frame = (sf::Uint32*) calloc(WINDOW_HEIGHT * WINDOW_WIDTH, sizeof(sf::Uint32));
    /*
    clock_t start_time = clock();

    for (size_t i = 0; i < 20; i++)
    {
        generate_image_by_pixel(frame);
    }

    clock_t end_time = clock();

    printf("pixel time: %ld\n", end_time - start_time);

    start_time = clock();

    for (size_t i = 0; i < 20; i++)
    {
        generate_image_by_line(frame);
    }

    end_time = clock();

    printf("line time:  %ld\n", end_time - start_time);
    */
    clock_t start_time = clock();

    for (size_t i = 0; i < 20; i++)
    {
        generate_image_by_simd(frame);
    }

    clock_t end_time = clock();

    printf("simd time:  %ld\n", end_time - start_time);

    sf::Texture tx;
    tx.create(WINDOW_WIDTH, WINDOW_HEIGHT);
    tx.update((sf::Uint8*) frame, WINDOW_WIDTH, WINDOW_HEIGHT, 0, 0);
 
    sf::Sprite sprite(tx);
    sprite.setPosition(0, 0);

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear();
        window.draw(sprite);
        window.display();
    }

    free(frame);

    return 0;
}
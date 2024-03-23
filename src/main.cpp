#include <stdio.h>
#include <SFML/Graphics.hpp>

#include "simd.h"

int main()
{

    sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Mandelbrot", sf::Style::Titlebar);

    sf::Uint32* frame = (sf::Uint32*) calloc(WINDOW_HEIGHT * WINDOW_WIDTH, sizeof(sf::Uint32));

    generate_image_by_pixel(frame);

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
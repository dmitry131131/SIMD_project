#include <stdio.h>
#include <SFML/Graphics.hpp>

#include "simd.h"

int main()
{

    sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Mandelbrot", sf::Style::Titlebar);

    sf::Uint32 arr[WINDOW_HEIGHT][WINDOW_WIDTH] = {};

    generate_image_by_pixel(arr);

    sf::Texture tx;
    tx.create(WINDOW_WIDTH, WINDOW_HEIGHT);
    tx.update((sf::Uint8*) arr, WINDOW_WIDTH, WINDOW_HEIGHT, 0, 0);
 
    sf::Sprite sprite(tx);
    sprite.setPosition(0, 0);

    // run the program as long as the window is open
    while (window.isOpen())
    {
        // check all the window's events that were triggered since the last iteration of the loop
        sf::Event event;
        while (window.pollEvent(event))
        {
            // "close requested" event: we close the window
            if (event.type == sf::Event::Closed)
                window.close();
        }
        window.clear(sf::Color::Black);
        window.draw(sprite);
        window.display();
    }

    return 0;
}
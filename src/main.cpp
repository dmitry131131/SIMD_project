#include <stdio.h>
#include <time.h>
#include <SFML/Graphics.hpp>

#include "simd.h"
#include "FPS.h"

int main()
{
    sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Mandelbrot", sf::Style::Titlebar);

    sf::Uint32* frame = (sf::Uint32*) calloc(WINDOW_HEIGHT * WINDOW_WIDTH, sizeof(sf::Uint32));
    float scale = 2.9f, X_offset = -0.25f, Y_offset = 0;

    #ifdef COMPARE_MODE
    
    compare_mode(frame, X_offset, Y_offset, scale, 180);

    #endif

    FPS_data FPS = {};

    sf::Text text;
    sf::Font font;
    font.loadFromFile("font/font.ttf");
    text.setFont(font);
    
    text.setCharacterSize(22);
    text.setFillColor(sf::Color::Red);
    text.setStyle(sf::Text::Bold);

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
            
            if (event.type == sf::Event::KeyPressed)
            {
                switch (event.key.code)
                {
                case sf::Keyboard::Left:
                    X_offset -= 0.05;
                    break;

                case sf::Keyboard::Right:
                    X_offset += 0.05;
                    break;

                case sf::Keyboard::Up:
                    Y_offset -= 0.05;
                    break;

                case sf::Keyboard::Down:
                    Y_offset += 0.05;
                    break;

                case sf::Keyboard::Dash:
                    scale += 0.05;
                    break;

                case sf::Keyboard::Equal:
                    scale -= 0.05;
                    break;
                
                default:
                    break;
                }
            }
        }

        calculate_FPS(&FPS, &text);

        window.clear();

        generate_image_by_simd(frame, X_offset, Y_offset, scale, 180);

        sf::Texture tx;
        tx.create(WINDOW_WIDTH, WINDOW_HEIGHT);
        tx.update((sf::Uint8*) frame, WINDOW_WIDTH, WINDOW_HEIGHT, 0, 0);
    
        sf::Sprite sprite(tx);
        sprite.setPosition(0, 0);

        window.draw(sprite);
        window.draw(text);

        (FPS.frame_counter++);

        window.display();
    }

    free(frame);
    return 0;
}
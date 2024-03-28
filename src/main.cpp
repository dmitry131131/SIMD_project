#include <stdio.h>
#include <time.h>
#include <SFML/Graphics.hpp>

#include "simd.h"
#include "FPS.h"

int main()
{
    sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Mandelbrot", sf::Style::Titlebar);

    render_context context = {};

    context_ctor(&context);

    #ifdef COMPARE_MODE

    sf::Text test_text;
    test_text.setFont(context.font);

    test_text.setCharacterSize(50);
    test_text.setFillColor(sf::Color::Green);
    test_text.setStyle(sf::Text::Bold);

    test_text.setString("Testing...");
    test_text.setPosition(WINDOW_WIDTH/2 - 150, WINDOW_HEIGHT/2 - 50);

    window.clear();
    window.draw(test_text);
    window.display();
    
    compare_mode(&context);

    #endif

    FPS_data FPS = {};

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
                    context.X_offset -= 0.05;
                    break;

                case sf::Keyboard::Right:
                    context.X_offset += 0.05;
                    break;

                case sf::Keyboard::Up:
                    context.Y_offset -= 0.05;
                    break;

                case sf::Keyboard::Down:
                    context.Y_offset += 0.05;
                    break;

                case sf::Keyboard::Dash:
                    context.scale += 0.05;
                    break;

                case sf::Keyboard::Equal:
                    context.scale -= 0.05;
                    break;

                case sf::Keyboard::Num1:
                    context.mode = PIXEL;
                    break;
                
                case sf::Keyboard::Num2:
                    context.mode = LINE;
                    break;

                case sf::Keyboard::Num3:
                    context.mode = SIMD;
                    break;

                default:
                    break;
                }
            }
        }

        calculate_FPS(&FPS, &context);

        window.clear();

        switch (context.mode)
        {
        case PIXEL:
            generate_image_by_pixel(&context);
            break;
        
        case LINE:
            generate_image_by_line(&context);
            break;

        case SIMD:
            generate_image_by_simd(&context);
            break;

        default:
            break;
        }

        sf::Texture tx;
        tx.create(WINDOW_WIDTH, WINDOW_HEIGHT);
        tx.update((sf::Uint8*) context.frame, WINDOW_WIDTH, WINDOW_HEIGHT, 0, 0);
    
        sf::Sprite sprite(tx);
        sprite.setPosition(0, 0);

        window.draw(sprite);
        window.draw(context.text);

        (FPS.frame_counter++);

        window.display();
    }

    context_dtor(&context);
    return 0;
}
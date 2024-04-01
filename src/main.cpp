#include <stdio.h>

#include <string.h>

#include <time.h>
#include <SFML/Graphics.hpp>

#include "simd.h"
#include "image.h"
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

    #ifdef CAT
    ImageData cat   = open_image("img/cat2.jpg");
    ImageData table = open_image("img/wall.jpg");

    ImageData out_data = img_data_cpy(&table);

    cat_compare_mode(&table, &cat, &out_data);

    save_image(&out_data, "final.jpg");
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
                    
                #ifndef CAT
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
                #endif

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

        #ifndef CAT

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

        #else

        switch (context.mode)
        {
        case PIXEL:
            make_composing_pixel(&table, &cat, &out_data);
            break;
        
        case LINE:
            make_composing_line(&table, &cat, &out_data);
            break;

        case SIMD:
            make_composing_simd(&table, &cat, &out_data);
            break;

        default:
            break;
        }

        sf::Texture table_texture = get_texture(&out_data);
        sf::Sprite  table_sprite(table_texture);
        table_sprite.setPosition(0, 0);

        window.draw(table_sprite);

        #endif

        window.draw(context.text);

        (FPS.frame_counter++);

        window.display();
    }

    #ifdef CAT
    free(cat.name);
    free(table.name);
    #endif

    context_dtor(&context);
    return 0;
}
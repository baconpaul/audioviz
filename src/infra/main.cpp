/*
 * AudioViz - just baconpaul noodling on stuff
 *
 * Copyright 2024, Paul Walker. Released under the MIT license.
 *
 * The images in the 'res' folder may be copyrighted and released
 * under restricted license. The code in scripts/ and src/ is all
 * MIT. But really, nothing to see here. Just a collaboration on
 * SFML for my current band
 *
 * All source for is available at
 * https://github.com/baconpaul/audioviz
 */
#include <SFML/Graphics.hpp>
#include <cstdint>

#include "glog.h"
#include "MainScreenProvider.h"

void utf32_to_utf8_string(uint32_t code, char *string)
{
    if (code < 0x80)
        string[0] = code;
    else if (code < 0x800)
    { // 00000yyy yyxxxxxx
        string[0] = (0b11000000 | (code >> 6));
        string[1] = (0b10000000 | (code & 0x3f));
    }
    else if (code < 0x10000)
    {                                                    // zzzzyyyy yyxxxxxx
        string[0] = (0b11100000 | (code >> 12));         // 1110zzz
        string[1] = (0b10000000 | ((code >> 6) & 0x3f)); // 10yyyyy
        string[2] = (0b10000000 | (code & 0x3f));        // 10xxxxx
    }
    else if (code < 0x200000)
    {                                                     // 000uuuuu zzzzyyyy yyxxxxxx
        string[0] = (0b11110000 | (code >> 18));          // 11110uuu
        string[1] = (0b10000000 | ((code >> 12) & 0x3f)); // 10uuzzzz
        string[2] = (0b10000000 | ((code >> 6) & 0x3f));  // 10yyyyyy
        string[3] = (0b10000000 | (code & 0x3f));         // 10xxxxxx
    }
}

int main()
{
    sf::ContextSettings settings;
    settings.antialiasingLevel = 4;

    auto window = sf::RenderWindow{{1024u, 768u}, "Audio Viz", sf::Style::Default, settings};
    window.setFramerateLimit(120);

    GLOG("Shaders available: " << sf::Shader::isAvailable());

    audioviz::MainScreenProvider ms(1024, 768);
    while (window.isOpen())
    {
        auto &cs = ms.currentScreen();
        bool returnToMainAtEnd{false};
        for (auto event = sf::Event{}; window.pollEvent(event);)
        {
            switch (event.type)
            {
            case sf::Event::Closed:
                window.close();
                break;
            case sf::Event::TextEntered:
            {
                char str[5]{0, 0, 0, 0};
                utf32_to_utf8_string(event.text.unicode, str);
                if (str[0] == 'Q' || str[0] == 'q')
                {
                    returnToMainAtEnd = true;
                }
                else
                {
                    cs->textEntered(str);
                }
            }
            break;
            case sf::Event::MouseButtonPressed:
            {
                cs->mouseDown(event.mouseButton.x, event.mouseButton.y);
            }
            default:
                break;
            }
        }

        window.clear();
        window.draw(*cs);
        window.display();
        cs->step();

        if (returnToMainAtEnd)
            ms.returnToMainMenu();
    }
}

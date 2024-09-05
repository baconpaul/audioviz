#include <SFML/Graphics.hpp>
#include <iostream>
#include <string>
#include <memory>
#include <cstdint>

#include "audioviz.h"
#include "TextureSet.h"
#include "LaserBeams.h"


int main()
{
    sf::ContextSettings settings;
    settings.antialiasingLevel = 8;

    auto window = sf::RenderWindow{{1024u, 768u}, "Audi Viz", sf::Style::Default, settings};
    window.setFramerateLimit(144);

    sf::Texture texture;
    if (!audioviz::texture::load("PirateSessions.png", texture))
    {
        GLOG("Failed to load texture");
        return 4;
    }
    GLOG("Texture loaded " << texture.getSize().x << " " << texture.getSize().y);

    sf::Sprite sprite;
    sprite.setTexture(texture);
    audioviz::graphics::LaserBeam lb;
    float ang{0};
    while (window.isOpen())
    {
        for (auto event = sf::Event{}; window.pollEvent(event);)
        {
            if (event.type == sf::Event::Closed)
            {
                window.close();
            }
        }

        window.clear();


        sf::VertexArray triangle(sf::Triangles, 3);

        // define the position of the triangle's points
        triangle[0].position = sf::Vector2f(10.f, 10.f);
        triangle[1].position = sf::Vector2f(200.f, 10.f);
        triangle[2].position = sf::Vector2f(200.f, 320.f);

        // define the color of the triangle's points
        triangle[0].color = sf::Color::White;
        triangle[1].color = sf::Color::White;
        triangle[2].color = sf::Color::White;

        triangle[0].texCoords = sf::Vector2f(0.f, 0.f);
        triangle[1].texCoords = sf::Vector2f(0.f, 400.f);
        triangle[2].texCoords = sf::Vector2f(400.f, 200.f * std::sin(ang));
        window.draw(triangle, &texture);

        window.draw(lb);

        auto x = std::sin(ang * 1.3) * 130 + 200;
        auto y = std::cos(ang * -0.9) * 180 + 190;
        ang += 0.01;
        sprite.setPosition(x + 100, y + 100);
        sprite.setScale(0.3, 0.3);
        window.draw(sprite);

        window.display();
        lb.step();
    }
}

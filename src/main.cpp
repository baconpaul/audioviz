#include <SFML/Graphics.hpp>
#include <iostream>
#include <string>
#include <memory>
#include <cstdint>

#include "cmrc/cmrc.hpp"

CMRC_DECLARE(audioviz_resources);


#define GLOG(...) std::cout << __FILE__ << ":" << __LINE__ << " [" << __func__ << "] " << __VA_ARGS__ << std::endl;

struct LaserBeam : sf::Drawable
{
    static constexpr int nPoints{100};
    LaserBeam() : m_vertices(sf::Points, nPoints) {
        for (int i=0; i<nPoints; ++i)
        {
            m_vertices[i].position = sf::Vector2f{ 500, 350 };
            m_angles[i] = sf::Vector2f( 4.0 * rand() / RAND_MAX - 2.0, 4.0 * rand() / RAND_MAX - 2.0);
        }
    }

    sf::VertexArray m_vertices;
    sf::Vector2f  m_angles[nPoints];
    void step()
    {
        for (int i=0; i<nPoints; ++i)
        {
            m_vertices[i].position += m_angles[i];
            if (m_vertices[i].position.x < 10 || m_vertices[i].position.x > 1000 ||
                m_vertices[i].position.y < 10 || m_vertices[i].position.y > 700)
            {
                m_vertices[i].position = sf::Vector2f{ 500, 350 };
                m_angles[i] = sf::Vector2f( 4.0 * rand() / RAND_MAX - 2.0, 4.0 * rand() / RAND_MAX - 2.0);
            }
        }
    }
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override
    {
        // our particles don't use a texture
        states.texture = nullptr;

        sf::CircleShape shape(3.f);

        for (int i=0; i<nPoints; ++i)
        {
            auto x = m_vertices[i].position.x;
            auto y = m_vertices[i].position.y;
            // set the shape color to green
            shape.setFillColor(sf::Color(190, 190, 240));
            shape.setPosition(x, y);
            // draw the vertex array
            target.draw(shape, states);
        }
    }

};

std::vector<uint8_t> getResource(const std::string &path)
{
    try
    {
        auto fs = cmrc::audioviz_resources::get_filesystem();
        auto df = fs.open(path);
        std::vector<uint8_t> dat(df.begin(), df.end());
        return dat;
    }
    catch (std::exception &e)
    {
    }
    GLOG("Resource '" << path << "' failed to load");
    return {};
}

int main()
{
    sf::ContextSettings settings;
    settings.antialiasingLevel = 8;

    auto window = sf::RenderWindow{{1024u, 768u}, "Audi Viz", sf::Style::Default, settings};
    window.setFramerateLimit(144);

    sf::Texture texture;
    auto ps = getResource("PirateSessions.png");
    if (ps.empty())
    {
        GLOG("Failed to acquire resource");
        return 2;
    }
    if (!texture.loadFromMemory((void *)&ps[0], ps.size()))
    {
        GLOG("Failed to load texture");
        return 4;
    }
    GLOG("Texture loaded " << texture.getSize().x << " " << texture.getSize().y);

    sf::Sprite sprite;
    sprite.setTexture(texture);
    LaserBeam lb;
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

        sf::CircleShape shape(50.f);

        // set the shape color to green
        shape.setFillColor(sf::Color(100, 250, 50));
        auto x = std::sin(ang) * 200 + 200;
        auto y = std::cos(ang) * 200 + 200;
        shape.setPosition(x, y);
        ang += 0.01;
        window.draw(shape);

        x = std::sin(ang * 1.3) * 130 + 200;
        y = std::cos(ang * -0.9) * 180 + 190;
        sprite.setPosition(x + 100, y + 100);
        window.draw(sprite);
        window.draw(lb);

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

        window.display();
        lb.step();
    }
}

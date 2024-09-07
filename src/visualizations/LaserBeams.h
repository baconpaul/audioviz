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

#ifndef AUDIOVIZ_SRC_VISUALIZATIONS_LASERBEAMS_H
#define AUDIOVIZ_SRC_VISUALIZATIONS_LASERBEAMS_H

#include <SFML/Graphics.hpp>
#include <cmath>
#include <cstdint>
#include <string>

#include "infra/resource_access.h"
#include "infra/glog.h"
#include "infra/screen.h"

namespace audioviz::graphics
{

struct LaserBeam : audioviz::infra::Screen
{
    std::string getName() const override { return "Lazer Beams, like Tempest"; }
    static constexpr int nPoints{100};
    static constexpr float cx{500}, cy{350};
    LaserBeam() : m_vertices(sf::Points, nPoints) {}
    void initialize() override
    {
        for (int i = 0; i < nPoints; ++i)
        {
            m_vertices[i].position = sf::Vector2f{cx, cy};
            m_angles[i] =
                sf::Vector2f(4.0 * rand() / RAND_MAX - 2.0, 4.0 * rand() / RAND_MAX - 2.0);
        }
        if (!audioviz::infra::load("BeamGradient.png", beamGrad))
        {
            GLOG("Failed to load beam gradient");
        }

        if (!audioviz::infra::load("PirateSessions.png", pirateSessions))
        {
            GLOG("Failed to load pirate sessions");
        }
        GLOG("PirateSessions loaded " << pirateSessions.getSize().x << " "
                                      << pirateSessions.getSize().y);
        GLOG("BeamGrad loaded " << beamGrad.getSize().x << " " << beamGrad.getSize().y);

        if (!audioviz::infra::load("pixel_operator/PixelOperatorMono-Bold.ttf", theFont))
        {
            GLOG("Unable to load font ");
        }

        pirateSprite.setTexture(pirateSessions);
    }

    float ang{0};
    sf::Font theFont;
    sf::Sprite pirateSprite;
    sf::Texture beamGrad, pirateSessions;
    sf::VertexArray m_vertices;
    sf::Vector2f m_angles[nPoints];
    void step() override
    {
        for (int i = 0; i < nPoints; ++i)
        {
            m_vertices[i].position += m_angles[i];
            if (m_vertices[i].position.x < 10 || m_vertices[i].position.x > cx * 2 - 20 ||
                m_vertices[i].position.y < 10 || m_vertices[i].position.y > cy * 2 - 20)
            {
                m_vertices[i].position = sf::Vector2f{cx, cy};
                m_angles[i] =
                    sf::Vector2f(4.0 * rand() / RAND_MAX - 2.0, 4.0 * rand() / RAND_MAX - 2.0);
            }
        }

        auto x = std::sin(ang * 1.3) * 130 + 200;
        auto y = std::cos(ang * -0.9) * 180 + 190;

        ang += 0.01;
        pirateSprite.setPosition(x + 100, y + 100);
        pirateSprite.setScale(0.3, 0.3);
    }

    bool isYFacing(int i) const
    {
        return fabs(m_vertices[i].position.x - cx) < fabs(m_vertices[i].position.y - cy);
    }

    void draw(sf::RenderTarget &target, sf::RenderStates states) const override
    {
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
        states.texture = &pirateSessions;
        target.draw(triangle, states);

        states.texture = &beamGrad;

        sf::VertexArray beam(sf::Triangles, 3);

        // define the position of the beam's points

        for (int i = 0; i < nPoints; ++i)
        {
            auto x = m_vertices[i].position.x;
            auto y = m_vertices[i].position.y;

            beam[0].position = sf::Vector2f(500.f, 350.f);
            auto dx = isYFacing(i) ? 10 : 0;
            auto dy = -(dx - 10);
            beam[1].position =
                sf::Vector2f(m_vertices[i].position.x - dx, m_vertices[i].position.y - dy);
            beam[2].position =
                sf::Vector2f(m_vertices[i].position.x + dx, m_vertices[i].position.y + dy);

            // define the color of the beam's points
            beam[0].color = sf::Color::White;
            beam[1].color = sf::Color::White;
            beam[2].color = sf::Color::White;

            beam[0].texCoords = sf::Vector2f(0.f, 0.f);
            beam[1].texCoords = sf::Vector2f(1072.f, 0.f);
            beam[2].texCoords = sf::Vector2f(1072.f, 0.f);
            target.draw(beam, states);
        }

        states.texture = nullptr;
        target.draw(pirateSprite, states);

        sf::Text text;

        text.setFont(theFont);
        text.setString("Leeward Visualization");
        text.setCharacterSize(50);

        auto b = text.getLocalBounds();
        auto cp = (target.getSize().x - b.getSize().x) * 0.5;

        text.setFillColor(sf::Color::Red);
        text.setPosition(cp + 3 + 50 * std::sin(ang), 3);
        target.draw(text, states);
        text.setFillColor(sf::Color::White);
        text.setPosition(cp + 50 * std::sin(ang), 0);
        target.draw(text, states);
    }
};

} // namespace audioviz::graphics
#endif // AUDIOVIZ_LASERBEAMS_H

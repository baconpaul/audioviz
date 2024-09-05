//
// Created by Paul Walker on 9/5/24.
//

#ifndef AUDIOVIZ_LASERBEAMS_H
#define AUDIOVIZ_LASERBEAMS_H


#include <SFML/Graphics.hpp>
#include "TextureSet.h"

namespace audioviz::graphics
{

struct LaserBeam : sf::Drawable
{
    static constexpr int nPoints{100};
    static constexpr float cx{500}, cy{350};
    LaserBeam() : m_vertices(sf::Points, nPoints) {
        for (int i=0; i<nPoints; ++i)
        {
            m_vertices[i].position = sf::Vector2f{ cx, cy};
            m_angles[i] = sf::Vector2f( 4.0 * rand() / RAND_MAX - 2.0, 4.0 * rand() / RAND_MAX - 2.0);
        }
        if (!audioviz::texture::load("BeamGradient.png", beamGrad))
        {
            GLOG("Probably not gonna work");
        }

        GLOG("Texture loaded " << beamGrad.getSize().x << " " << beamGrad.getSize().y);

    }

    sf::Texture beamGrad;
    sf::VertexArray m_vertices;
    sf::Vector2f  m_angles[nPoints];
    void step()
    {
        for (int i=0; i<nPoints; ++i)
        {
            m_vertices[i].position += m_angles[i];
            if (m_vertices[i].position.x < 10 || m_vertices[i].position.x > cx * 2 - 20 ||
                m_vertices[i].position.y < 10 || m_vertices[i].position.y > cy * 2 - 20)
            {
                m_vertices[i].position = sf::Vector2f{ cx, cy };
                m_angles[i] = sf::Vector2f( 4.0 * rand() / RAND_MAX - 2.0, 4.0 * rand() / RAND_MAX - 2.0);
            }
        }
    }

    bool isYFacing(int i) const
    {
        return fabs(m_vertices[i].position.x-cx) < fabs(m_vertices[i].position.y - cy);
    }

    void draw(sf::RenderTarget& target, sf::RenderStates states) const override
    {
        // our particles don't use a texture
        states.texture = &beamGrad;

        sf::VertexArray beam(sf::Triangles, 3);

        // define the position of the beam's points

        for (int i=0; i<nPoints; ++i)
        {
            auto x = m_vertices[i].position.x;
            auto y = m_vertices[i].position.y;

            beam[0].position = sf::Vector2f(500.f, 350.f);
            auto dx = isYFacing(i) ? 10 : 0;
            auto dy = - ( dx - 10);
            beam[1].position = sf::Vector2f(m_vertices[i].position.x - dx, m_vertices[i].position.y - dy);
            beam[2].position = sf::Vector2f(m_vertices[i].position.x + dx, m_vertices[i].position.y + dy);

            // define the color of the beam's points
            beam[0].color = sf::Color::White;
            beam[1].color = sf::Color::White;
            beam[2].color = sf::Color::White;

            beam[0].texCoords = sf::Vector2f(0.f, 0.f);
            beam[1].texCoords = sf::Vector2f(1072.f, 0.f);
            beam[2].texCoords = sf::Vector2f(1072.f, 0.f );
            target.draw(beam, states);

        }
    }

};

}
#endif // AUDIOVIZ_LASERBEAMS_H

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

#ifndef AUDIOVIZ_SRC_VISUALIZATIONS_SHADERTESTTWO_H
#define AUDIOVIZ_SRC_VISUALIZATIONS_SHADERTESTTWO_H

#include <SFML/Graphics.hpp>
#include <cmath>
#include <cstdint>
#include <string>

#include "infra/resource_access.h"
#include "infra/glog.h"
#include "infra/screen.h"

namespace audioviz::graphics
{

struct ShaderTestTwo : audioviz::infra::Screen
{
    std::string getName() const override { return "Shader Test w Audio and Verts"; }
    ShaderTestTwo() {}

    void initialize(int w, int h) override
    {
        if (!shaderLoaded)
        {
            auto s = audioviz::infra::shaderSource("valuenoise.frag");
            shader.loadFromMemory(s, sf::Shader::Fragment);
        }
        shaderLoaded = true;
    }

    bool shaderLoaded{false};
    sf::Shader shader;
    float ang{0.f};

    void step() override
    {
        auto timev = std::cbrt(audioSystem->level.load()) * 2.f * 3.1415f;
        shader.setUniform("u_time", timev);
        shader.setUniform("u_resolution", sf::Vector2f{(float)500 + 10 * std::sin(timev),
                                                       (float)320 + 10 * std::sin(timev)});
        ang = timev;
    }

    void draw(sf::RenderTarget &target, sf::RenderStates states) const override
    {
        states.shader = &shader;
        sf::VertexArray triangle(sf::Triangles, 3);

        // define the position of the triangle's points
        triangle[0].position = sf::Vector2f(10.f, 10.f);
        triangle[1].position = sf::Vector2f(200.f, 10.f);
        triangle[2].position =
            sf::Vector2f(200.f + 400 * std::sin(ang) * std::sin(ang), 320.f + 100 * std::sin(ang));

        // define the color of the triangle's points
        triangle[0].color = sf::Color::White;
        triangle[1].color = sf::Color::White;
        triangle[2].color = sf::Color::White;

        target.draw(triangle, states);
    }
};

} // namespace audioviz::graphics
#endif // AUDIOVIZ_LASERBEAMS_H

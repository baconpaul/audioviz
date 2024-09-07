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

#ifndef AUDIOVIZ_SRC_VISUALIZATIONS_SHADERTEST_H
#define AUDIOVIZ_SRC_VISUALIZATIONS_SHADERTEST_H

#include <SFML/Graphics.hpp>
#include <cmath>
#include <cstdint>
#include <string>

#include "infra/resource_access.h"
#include "infra/glog.h"
#include "infra/screen.h"

namespace audioviz::graphics
{

struct ShaderTest : audioviz::infra::Screen
{
    std::string getName() const override { return "Shader Test"; }
    ShaderTest() {}

    void initialize() override
    {
        if (!shaderLoaded)
        {
            auto s = audioviz::infra::shaderSource("valuenoise.frag");
            shader.loadFromMemory(s, sf::Shader::Fragment);
        }
        shaderLoaded = true;
        shader.setUniform("u_resolution", sf::Vector2f{(float)width, (float)height});
        shader.setUniform("u_time", time);
        texture.create(width, height);
    }

    bool shaderLoaded{false};
    sf::Shader shader;
    sf::Texture texture;
    float time{0.f};

    void step() override
    {
        time = time + 0.005f;
        shader.setUniform("u_time", time);
    }

    void draw(sf::RenderTarget &target, sf::RenderStates states) const override
    {
        states.shader = &shader;
        sf::Sprite sprite(texture);

        target.draw(sprite, states);
    }
};

} // namespace audioviz::graphics
#endif // AUDIOVIZ_LASERBEAMS_H

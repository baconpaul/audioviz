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

#ifndef AUDIOVIZ_SRC_VISUALIZATIONS_KUNGFU_H
#define AUDIOVIZ_SRC_VISUALIZATIONS_KUNGFU_H

#include <SFML/Graphics.hpp>
#include <cmath>
#include <cstdint>
#include <string>

#include "infra/resource_access.h"
#include "infra/glog.h"
#include "infra/screen.h"
#include <array>

namespace audioviz::graphics
{

struct KungFu : audioviz::infra::Screen
{
    std::string getName() const override { return "Kung Fun?"; }
    KungFu() {}

    static constexpr size_t nSprites{20};
    sf::Texture kungFu;
    std::array<sf::Sprite, nSprites> kfSprite;
    std::array<sf::Vector2f, nSprites> velocity;

    void initialize() override
    {
        audioviz::infra::load("BriansYieArKungFu.png", kungFu);
        for (auto &s : kfSprite)
            s = sf::Sprite(kungFu);
        count = 0;
    }

    static constexpr int spriteSz{40};
    int count = 0;
    void step() override
    {
        if (count == 0)
        {
            for (auto &s : kfSprite)
            {
                int ir = rand() % 7;
                int jr = rand() % 3;
                s.setTextureRect({ir * spriteSz, jr * spriteSz, spriteSz, spriteSz - 1});

                int xp = rand() % (width - spriteSz);
                int yp = rand() % (height - spriteSz);
                s.setPosition(xp, yp);
                float scale = (rand() % 100) / 50.0 + 0.5;
                s.setScale(scale, scale);
            }
            for (auto &s : velocity)
            {
                auto r1 = (rand() % 1000 - 500) / 700.f;
                auto r2 = (rand() % 1000 - 500) / 700.f;
                s = {r1, r2};
            }
        }

        for (int i = 0; i < nSprites; ++i)
        {
            float lv = std::cbrt(audioSystem->level.load() * 5) + 0.01;
            auto p = kfSprite[i].getPosition() + velocity[i] * lv;
            kfSprite[i].setPosition(p);
        }

        count++;
        if (count == 480)
            count = 0;
    }

    void draw(sf::RenderTarget &target, sf::RenderStates states) const override
    {
        states.texture = &kungFu;
        for (auto &s : kfSprite)
        {
            target.draw(s, states);
        }
    }
};

} // namespace audioviz::graphics
#endif // AUDIOVIZ_LASERBEAMS_H

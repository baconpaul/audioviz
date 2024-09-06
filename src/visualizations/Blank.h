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

#ifndef AUDIOVIZ_SRC_VISUALIZATIONS_BLANK_H
#define AUDIOVIZ_SRC_VISUALIZATIONS_BLANK_H

#include <SFML/Graphics.hpp>
#include <cmath>
#include <cstdint>
#include <string>

#include "infra/resource_access.h"
#include "infra/glog.h"
#include "infra/screen.h"
#include <chrono>

namespace audioviz::graphics
{

struct Blank : audioviz::infra::Screen
{
    std::string getName() const override { return "Blank Starting Point"; }
    Blank() {}

    std::chrono::high_resolution_clock::time_point lasttime{};

    void initialize(int w, int h) override
    {
        audioviz::infra::loadPixelFont(theFont);
        counter = 0;
        lasttime = std::chrono::high_resolution_clock::now();
    }

    sf::Font theFont;
    int counter{0};
    uint64_t frameRate;
    void step() override
    {
        counter++;
        auto now = std::chrono::high_resolution_clock::now();
        auto dframe = now - lasttime;
        lasttime = now;

        auto msec = std::chrono::duration_cast<std::chrono::microseconds>(dframe);
        frameRate = 1000000 / msec.count();
    }

    void draw(sf::RenderTarget &target, sf::RenderStates states) const override
    {
        sf::Text text;

        text.setFont(theFont);
        text.setCharacterSize(100);

        text.setString(std::to_string(counter));

        auto b = text.getLocalBounds();
        auto cp = (target.getSize().x - b.getSize().x) * 0.5;

        text.setFillColor(
            sf::Color(255, 200 + 50 * sin(counter * 0.001), 200 + 50 * cos(counter * 0.00074)));
        text.setPosition(cp, 3);
        target.draw(text, states);

        text.setString(std::to_string(frameRate) + " fps (lim 120)");

        b = text.getLocalBounds();
        cp = (target.getSize().x - b.getSize().x) * 0.5;

        text.setPosition(cp, 120);
        target.draw(text, states);
    }
};

} // namespace audioviz::graphics
#endif // AUDIOVIZ_LASERBEAMS_H

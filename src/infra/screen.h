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

#ifndef AUDIOVIZ_SRC_INFRA_SCREEN_H
#define AUDIOVIZ_SRC_INFRA_SCREEN_H

#include <SFML/Graphics.hpp>
#include <memory>
#include <string>

namespace audioviz::infra
{
struct Screen : sf::Drawable
{
    virtual ~Screen() = default;
    virtual void step() = 0;
    virtual void mouseDown(int x, int y) {}
    virtual void textEntered(const std::string &c) {}
};

struct ScreenProvider
{
    virtual ~ScreenProvider() = default;
    virtual const std::unique_ptr<Screen> &currentScreen() const = 0;
};
} // namespace audioviz::infra

#endif // AUDIOVIZ_SCREEN_H

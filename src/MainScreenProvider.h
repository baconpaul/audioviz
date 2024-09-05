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

#ifndef AUDIOVIZ_SRC_MAINSCREENPROVIDER_H
#define AUDIOVIZ_SRC_MAINSCREENPROVIDER_H

#include <unordered_map>
#include <memory>
#include <string>

#include "infra/screen.h"

namespace audioviz
{
struct MainScreenProvider : infra::ScreenProvider
{
    std::unordered_map<std::string, std::unique_ptr<infra::Screen>> screens;
    MainScreenProvider();
    const std::unique_ptr<infra::Screen> &currentScreen() const override;
};
} // namespace audioviz

#endif // AUDIOVIZ_MAINSCREEN_H

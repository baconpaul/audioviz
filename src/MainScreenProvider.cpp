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

#include "MainScreenProvider.h"
#include "visualizations/LaserBeams.h"

namespace audioviz
{
MainScreenProvider::MainScreenProvider()
{
    screens["laserbeams"] = std::make_unique<audioviz::graphics::LaserBeam>();
}

const std::unique_ptr<infra::Screen> &MainScreenProvider::currentScreen() const
{
    return screens.at("laserbeams");
}
} // namespace audioviz
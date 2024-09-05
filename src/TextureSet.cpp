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

#include "TextureSet.h"
#include "audioviz.h"
#include "cmrc/cmrc.hpp"

CMRC_DECLARE(audioviz_resources);

namespace audioviz::texture
{
bool load(const std::string &path, sf::Texture &into)
{
    try
    {
        auto fs = cmrc::audioviz_resources::get_filesystem();
        auto df = fs.open(path);
        return into.loadFromMemory(df.begin(), df.size());
    }
    catch (std::exception &e)
    {
    }
    GLOG("Resource '" << path << "' failed to load");
    return false;
}
} // namespace audioviz::texture
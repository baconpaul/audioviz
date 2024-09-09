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

#include "resource_access.h"
#include "glog.h"
#include "cmrc/cmrc.hpp"

CMRC_DECLARE(audioviz_resources);
CMRC_DECLARE(audioviz_shaders);

namespace audioviz::infra
{
template <typename T> bool loadImpl(const std::string &path, T &into)
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

bool load(const std::string &path, sf::Texture &into) { return loadImpl(path, into); }

bool load(const std::string &path, sf::Font &into) { return loadImpl(path, into); }

bool loadPixelFont(sf::Font &into)
{
    return load("pixel_operator/PixelOperatorMono-Bold.ttf", into);
}
bool loadMonoFont(sf::Font &into) { return load("AnonymousPro/AnonymousPro-Regular.ttf", into); }

std::string shaderSource(const std::string &path)
{
    try
    {
        auto fs = cmrc::audioviz_shaders::get_filesystem();
        auto df = fs.open(path);
        return std::string(df.begin(), df.size());
    }
    catch (std::exception &e)
    {
    }
    GLOG("Resource '" << path << "' failed to load");

    return {};
}

std::vector<std::string> allShaders()
{
    try
    {
        std::vector<std::string> res;
        auto fs = cmrc::audioviz_shaders::get_filesystem();
        for (auto ent : fs.iterate_directory(""))
        {
            GLOG(ent.filename());
            res.push_back(ent.filename());
        }
        return res;
    }
    catch (std::exception &e)
    {
    }

    return {};
}
} // namespace audioviz::infra
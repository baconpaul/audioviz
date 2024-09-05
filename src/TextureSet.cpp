//
// Created by Paul Walker on 9/5/24.
//


#include "TextureSet.h"
#include "audioviz.h"
#include "cmrc/cmrc.hpp"

CMRC_DECLARE(audioviz_resources);

namespace audioviz::texture
{
bool load(const std::string &path, sf::Texture &into) {
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
}
//
// Created by Paul Walker on 9/5/24.
//

#ifndef AUDIOVIZ_TEXTURESET_H
#define AUDIOVIZ_TEXTURESET_H

#include <SFML/Graphics.hpp>

namespace audioviz::texture
{
bool load(const std::string &path, sf::Texture &into);
}

#endif // AUDIOVIZ_TEXTURESET_H

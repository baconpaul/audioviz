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

#ifndef AUDIOVIZ_SRC_VISUALIZATIONS_LOCALSHADER_H
#define AUDIOVIZ_SRC_VISUALIZATIONS_LOCALSHADER_H

#include <SFML/Graphics.hpp>
#include <cmath>
#include <cstdint>
#include <string>

#include "infra/resource_access.h"
#include "infra/glog.h"
#include "infra/screen.h"
#include <filesystem>
#include <fstream>

namespace audioviz::graphics
{

struct LocalShader : audioviz::infra::Screen
{
    std::string getName() const override { return "Local Shader"; }
    LocalShader() {}

    void initialize() override
    {
        audioviz::infra::loadPixelFont(font);
        if (!shaderLoaded)
        {
            auto s = audioviz::infra::shaderSource("most_basic.frag");
            shader.loadFromMemory(s, sf::Shader::Fragment);
            try
            {
                auto p = std::filesystem::path{getenv("HOME")} / "Desktop";
                if (std::filesystem::is_directory(p))
                {
                    GLOG("Found desktop");
                    auto f = p / "audioviz_shader.frag";
                    if (!std::filesystem::is_regular_file(f))
                    {
                        GLOG("Creating audioviz shader frag");
                        std::ofstream of(f);
                        of << s;
                        of.close();
                    }
                    shaderPath = f;
                    reloadFromShaderPath();
                }
            }
            catch (std::filesystem::filesystem_error &)
            {
            }
        }
        shaderLoaded = true;
        time = 0.f;
        texture.create(width, height);
        msgFrames = 120 * 4;
    }

    void textEntered(const std::string &c) override
    {
        if (std::tolower(c[0]) == 'r')
            reloadFromShaderPath();
    }
    void reloadFromShaderPath()
    {
        GLOG("Reloading from " << shaderPath.u8string());
        std::ifstream ifs(shaderPath);
        std::stringstream buffer;
        buffer << ifs.rdbuf();
        shader.loadFromMemory(buffer.str(), sf::Shader::Fragment);
    }

    std::filesystem::path shaderPath{};

    bool shaderLoaded{false};
    sf::Shader shader;
    sf::Texture texture;
    sf::Font font;
    float time{0.f};
    int msgFrames;

    void step() override
    {
        time = time + 0.005f;
        shader.setUniform("u_resolution", sf::Vector2f{(float)width, (float)height});
        shader.setUniform("u_time", time);
        msgFrames = std::max(msgFrames - 1, 0);
    }

    void draw(sf::RenderTarget &target, sf::RenderStates states) const override
    {
        states.shader = &shader;
        sf::Sprite sprite(texture);

        target.draw(sprite, states);

        if (msgFrames > 0)
        {
            sf::Text tx;
            tx.setFont(font);
            tx.setCharacterSize(50);
            tx.setString("Press R to reload from file");
            tx.setFillColor(sf::Color::Red);
            tx.setPosition(10, 13);
            states.shader = nullptr;
            target.draw(tx, states);
            tx.setFillColor(sf::Color::White);
            tx.setPosition(7, 10);
            target.draw(tx, states);
        }
    }
};

} // namespace audioviz::graphics
#endif // AUDIOVIZ_LASERBEAMS_H

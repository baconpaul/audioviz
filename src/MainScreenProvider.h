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

#include <map>
#include <memory>
#include <string>
#include <tuple>
#include <vector>

#include "infra/screen.h"
#include "audio/audio.h"

namespace audioviz
{

struct MainScreenProvider : infra::ScreenProvider
{
    std::shared_ptr<audio::AudioSystem> audioSystem;
    std::map<std::string, std::unique_ptr<infra::Screen>> screens;
    std::unique_ptr<infra::Screen> menuScreen;
    std::unique_ptr<infra::Screen> shaderLibraryScreen;
    MainScreenProvider(int w, int h);
    ~MainScreenProvider();
    const std::unique_ptr<infra::Screen> &currentScreen() const override;
    void returnToMainMenu();
    void setCurrentScreen(const std::string &s);
    void showShader(const std::string &path);

  private:
    std::string cs{};
    int width, height;
};
} // namespace audioviz

#endif // AUDIOVIZ_MAINSCREEN_H

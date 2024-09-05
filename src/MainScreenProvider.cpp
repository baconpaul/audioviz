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
#include <deque>
#include <tuple>
#include <vector>

namespace audioviz
{
struct MenuScreen : infra::Screen
{
    using action_t = std::tuple<char, std::string, std::function<void()>>;
    using screenAction_t = std::vector<action_t>;
    using screenStack_t = std::deque<screenAction_t>;

    screenStack_t stack;

    sf::Font screenFont;
    MainScreenProvider &mainScreen;

    MenuScreen(MainScreenProvider &ms) : mainScreen(ms)
    {
        auto main = screenAction_t();
        main.emplace_back('S', "Settings", [this]() { pushSettingsMenu(); });
        main.emplace_back('1', "Lazer Beams", [this]() {
            GLOG("Lazer Beams");
            this->mainScreen.cs = "laserbeams";
        });
        stack.push_back(main);

        if (!audioviz::infra::load("pixel_operator/PixelOperatorMono-Bold.ttf", screenFont))
        {
            GLOG("Unable to load font ");
        }
    }

    void pushSettingsMenu()
    {
        GLOG("Push Settings Menu");
        auto main = screenAction_t();
        main.emplace_back('1', "Coming", [this]() { GLOG("Coming"); });
        main.emplace_back('2', "Soon", [this]() { GLOG("Soon"); });
        main.emplace_back('3', "Back to Main", [this]() {
            this->stack.pop_back();
        });
        stack.push_back(main);
    }
    void step() override {}

    void textEntered(const std::string &cp) override
    {
        const auto &tp = stack.back();
        auto ht{30};
        auto ps{10};
        for (auto [c, l, f] : tp)
        {
            if (std::tolower(cp[0]) == std::tolower(c))
            {
                f();
            }
        }
    }

    void draw(sf::RenderTarget &target, sf::RenderStates states) const override
    {
        const auto &tp = stack.back();
        sf::Text txt;
        auto ht{30};

        txt.setFont(screenFont);
        txt.setPosition(10, 10);
        txt.setFillColor(sf::Color(200, 200, 255));
        txt.setCharacterSize(ht);
        txt.setString("Leeward Visualization Control Panel");
        target.draw(txt, states);
        txt.setString("Select an option below. From a viz, press 'Q' to return to menu");
        txt.setPosition(10, 10 + ht);
        target.draw(txt, states);

        auto ps{20 + 2 * ht};

        for (auto [c, l, f] : tp)
        {
            sf::Text txt;
            txt.setFont(screenFont);
            txt.setPosition(10, ps);
            txt.setString(c);
            txt.setFillColor(sf::Color(255, 255, 0));
            txt.setCharacterSize(ht);
            target.draw(txt, states);

            txt.setString(l);
            txt.setPosition(40, ps);
            txt.setFillColor(sf::Color::White);
            target.draw(txt, states);

            ps += ht;
        }
    }
};

MainScreenProvider::MainScreenProvider()
{
    screens["mainmenu"] = std::make_unique<MenuScreen>(*this);
    screens["laserbeams"] = std::make_unique<audioviz::graphics::LaserBeam>();
    cs = "mainmenu";
}

const std::unique_ptr<infra::Screen> &MainScreenProvider::currentScreen() const
{
    return screens.at(cs);
}

void MainScreenProvider::returnToMainMenu() { cs = "mainmenu"; }
} // namespace audioviz
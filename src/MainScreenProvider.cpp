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
#include <deque>
#include <tuple>
#include <vector>
#include <functional>
#include <string>

#include "visualizations/LaserBeams.h"
#include "visualizations/ShaderTest.h"

namespace audioviz
{
struct MenuScreen : infra::Screen
{
    std::string getName() const override { return "Menu Screen"; }
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

        char c = '1';
        for (const auto &[k, s] : ms.screens)
        {
            main.emplace_back(c, s->getName(),
                              [this, sc = k]() { this->mainScreen.setCurrentScreen(sc); });
            c++;
            if (c > '9')
                c = 'A';
        }
        stack.push_back(main);

        if (!audioviz::infra::load("pixel_operator/PixelOperatorMono-Bold.ttf", screenFont))
        {
            GLOG("Unable to load font ");
        }
    }

    void pushSettingsMenu()
    {
        auto main = screenAction_t();
        main.emplace_back('1', "Coming", [this]() { GLOG("Coming"); });
        main.emplace_back('2', "Soon", [this]() { GLOG("Soon"); });
        main.emplace_back('3', "Back to Main", [this]() { this->stack.pop_back(); });
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

MainScreenProvider::MainScreenProvider(int w, int h) : width(w), height(h)
{
    screens["laserbeams"] = std::make_unique<audioviz::graphics::LaserBeam>();
    screens["shadertest"] = std::make_unique<audioviz::graphics::ShaderTest>();

    // Set this up after the other screens
    menuScreen = std::make_unique<MenuScreen>(*this);
    cs = "mainmenu";
}

MainScreenProvider::~MainScreenProvider() = default;

const std::unique_ptr<infra::Screen> &MainScreenProvider::currentScreen() const
{
    auto sf = screens.find(cs);
    if (sf == screens.end() || cs == "mainmenu")
        return menuScreen;
    return sf->second;
}

void MainScreenProvider::setCurrentScreen(const std::string &s)
{
    GLOG("setCurrentScreen '" << s << "'");
    auto sf = screens.find(s);
    if (sf == screens.end())
    {
        GLOG("That's screens.end so make it main menu");
        cs = "mainmenu";
        return;
    }

    cs = s;
    sf->second->initialize(width, height);
}

void MainScreenProvider::returnToMainMenu() { cs = "mainmenu"; }
} // namespace audioviz
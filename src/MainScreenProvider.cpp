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
#include <sstream>

#include "audio/audio.h"

#include "visualizations/LaserBeams.h"
#include "visualizations/ShaderTest.h"
#include "visualizations/ShaderTestTwo.h"

namespace audioviz
{
struct MenuScreen : infra::Screen
{
    std::string getName() const override { return "Menu Screen"; }
    using action_t = std::tuple<char, std::string, std::function<void()>>;
    using screenAction_t = std::vector<action_t>;
    using screenStack_t = std::deque<screenAction_t>;

    screenStack_t stack;

    sf::Font pixelFont, textFont;
    MainScreenProvider &mainScreen;
    std::string versionString;

    MenuScreen(MainScreenProvider &ms) : mainScreen(ms) {}
    void initialize(int w, int h) override
    {
        auto main = screenAction_t();
        // Toggle depends on this being the first item, below. Gross but hey thats ok
        main.emplace_back('Y', "Start Audio Input", [this]() { toggleAudioInput(); });
        main.emplace_back('Z', "Select Audio Input", [this]() { pushAudioInputMenu(); });

        char c = '1';
        for (const auto &[k, s] : mainScreen.screens)
        {
            main.emplace_back(c, s->getName(),
                              [this, sc = k]() { this->mainScreen.setCurrentScreen(sc); });
            c++;
            if (c > '9')
                c = 'A';
        }
        stack.push_back(main);

        if (!(audioviz::infra::loadPixelFont(pixelFont) && audioviz::infra::loadMonoFont(textFont)))
        {
            GLOG("Unable to load fonts");
        }

        std::ostringstream vs;
        vs << "Build: " << GIT_HASH << " " << __DATE__ << " " << __TIME__;
        versionString = vs.str();
    }

    void toggleAudioInput()
    {
        if (audioSystem->isRunning())
        {
            audioSystem->stop();
            std::get<1>(stack.front()[0]) = "Start Audio Input";
        }
        else
        {
            audioSystem->start();
            std::get<1>(stack.front()[0]) = "Stop Audio Input";
        }
    }
    void pushAudioInputMenu()
    {
        auto dev = audioSystem->inputDevices();
        auto main = screenAction_t();
        char c = '1';
        main.emplace_back('U', "Up to main menu", [this]() { doPopBackAsap(); });
        for (auto &[didx, nm] : dev)
        {
            auto idx = c - '1';
            main.emplace_back(c, nm, [this, d = didx]() {
                doPopBackAsap();
                audioSystem->selectInput(d);
            });
            c = c + 1;
        }
        stack.push_back(main);
    }
    float time{0.f};
    void step() override { time += 0.01; }

    bool dpb{false};
    void doPopBackAsap() { dpb = true; }
    void textEntered(const std::string &cp) override
    {
        const auto &tp = stack.back();
        dpb = false;
        for (auto [c, l, f] : tp)
        {
            if (std::tolower(cp[0]) == std::tolower(c))
            {
                f();
            }
        }
        if (dpb)
            stack.pop_back();
    }

    void draw(sf::RenderTarget &target, sf::RenderStates states) const override
    {
        if (stack.empty())
            return;

        const auto &tp = stack.back();
        sf::Text txt;
        auto ht{40};
        auto ypos{-5};
        int pad{3};
        txt.setFont(pixelFont);
        txt.setPosition(10, ypos);
        txt.setFillColor(
            sf::Color(200 + 50 * std::cos(1.2 * time), 200 + 50 * std::sin(time), 255));
        txt.setCharacterSize(ht);
        txt.setString("Leeward Visualization Control Panel");
        target.draw(txt, states);
        ypos += ht + 5;

        ht = 16;

        txt.setFont(textFont);
        txt.setCharacterSize(ht);
        txt.setFillColor(sf::Color::Yellow);
        txt.setString("Select an option below. From a viz, press 'Q' to return to menu");
        txt.setPosition(10, ypos);
        ypos += ht + pad;
        target.draw(txt, states);

        txt.setPosition(10, ypos);
        txt.setString("Audio device: " + audioSystem->selectedDevice.second);
        ypos += ht + pad;
        target.draw(txt, states);

        ypos += 8;

        auto ps{ypos};

        for (auto [c, l, f] : tp)
        {
            sf::Text txt;
            txt.setFont(textFont);
            txt.setPosition(10, ps);
            txt.setString(c);
            txt.setFillColor(sf::Color(255, 255, 0));
            txt.setCharacterSize(ht);
            target.draw(txt, states);

            txt.setString(l);
            txt.setPosition(40, ps);
            txt.setFillColor(sf::Color::White);
            target.draw(txt, states);

            ps += ht + pad;
        }

        auto fpos = target.getSize().y - 10 - ht;
        txt.setPosition(10, fpos);
        txt.setFillColor(sf::Color(0, 255, 0));

        txt.setString(versionString);
        txt.setCharacterSize(ht);
        target.draw(txt, states);

        int rad{80};
        sf::CircleShape cs(rad);
        cs.setFillColor(sf::Color(80, 80, 80));
        cs.setPosition(target.getSize().x - 2 * rad - 10, 10);
        target.draw(cs, states);

        auto lv = audioSystem->level.load();
        auto lrad = rad * std::min(1.f, std::cbrt(lv));
        sf::CircleShape clev(lrad);
        auto rdiff = rad - lrad;
        clev.setFillColor(sf::Color(250, 80, 80));
        clev.setPosition(target.getSize().x - 2 * rad - 10 + rdiff, 10 + rdiff);
        target.draw(clev, states);
    }
};

MainScreenProvider::MainScreenProvider(int w, int h) : width(w), height(h)
{
    audioSystem = std::make_shared<audio::AudioSystem>();
    screens["laserbeams"] = std::make_unique<audioviz::graphics::LaserBeam>();
    screens["shadertest"] = std::make_unique<audioviz::graphics::ShaderTest>();
    screens["shadertesttwo"] = std::make_unique<audioviz::graphics::ShaderTestTwo>();

    for (auto &[k, s] : screens)
        s->audioSystem = audioSystem;

    // Set this up after the other screens
    menuScreen = std::make_unique<MenuScreen>(*this);
    menuScreen->audioSystem = audioSystem;
    menuScreen->initialize(width, height);
    setCurrentScreen("mainmenu");
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
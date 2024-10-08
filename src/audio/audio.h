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

#ifndef AUDIOVIZ_SRC_AUDIO_AUDIO_H
#define AUDIOVIZ_SRC_AUDIO_AUDIO_H

#include <vector>
#include <string>
#include <utility>
#include <atomic>
#include <memory>

class RtAudio;

namespace audioviz::audio
{
struct AudioSystem
{
    AudioSystem();
    ~AudioSystem();
    using device_t = std::pair<int, std::string>;
    std::vector<device_t> inputDevices();
    device_t defaultInputDevice();
    void selectInput(int idx);

    bool isRunning() const;
    void start();
    void stop();

    void process(void *inData, int nFrames);

    device_t selectedDevice;

    std::atomic<float> level;

  private:
    std::unique_ptr<RtAudio> session;
    int nChannels{0};
};
} // namespace audioviz::audio
#endif // AUDIOVIZ_AUDIO_H

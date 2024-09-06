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

#include "audio.h"

#include "RtAudio.h"

#include "infra/glog.h"

namespace audioviz::audio
{
AudioSystem::AudioSystem()
{
    session = std::make_unique<RtAudio>();
    selectedDevice = defaultInputDevice();
}
AudioSystem::~AudioSystem() = default;

std::vector<AudioSystem::device_t> AudioSystem::inputDevices()
{
    GLOG("Listing input devices");
    auto &audio = *session;
    // Get the list of device IDs
    auto ids = audio.getDeviceIds();
    if (ids.size() == 0)
    {
        GLOG("No devices found.");
        return {};
    }

    // Scan through devices for various capabilities
    RtAudio::DeviceInfo info;
    std::vector<AudioSystem::device_t> res;
    for (unsigned int n = 0; n < ids.size(); n++)
    {
        info = audio.getDeviceInfo(ids[n]);
        if (info.inputChannels > 0)
        {
            res.push_back({n, info.name});
        }
    }
    return res;
}

AudioSystem::device_t AudioSystem::defaultInputDevice()
{
    auto &audio = *session;
    // Get the list of device IDs
    std::vector<unsigned int> ids = audio.getDeviceIds();
    if (ids.size() == 0)
    {
        GLOG("No devices found.");
        return {};
    }

    // Scan through devices for various capabilities
    RtAudio::DeviceInfo info;
    for (unsigned int n = 0; n < ids.size(); n++)
    {
        info = audio.getDeviceInfo(ids[n]);

        if (info.isDefaultInput)
        {
            return {n, info.name};
        }
    }
    return {};
}

void AudioSystem::selectInput(int idx)
{
    auto &audio = *session;
    // Get the list of device IDs
    std::vector<unsigned int> ids = audio.getDeviceIds();
    auto theInfo = audio.getDeviceInfo(ids[idx]);
    selectedDevice = {idx, theInfo.name};
}
} // namespace audioviz::audio
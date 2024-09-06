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

void rtaErrorCallback(RtAudioErrorType errorType, const std::string &errorText)
{
    GLOG("[ERROR] RtAudio reports '" << errorText << "'"
                                     << " " << errorType);
}

int rtaCallback(void *outputBuffer, void *inputBuffer, unsigned int nBufferFrames,
                double /* streamTime */, RtAudioStreamStatus status, void *data)
{
    if (status)
    {
    }

    auto asys = (AudioSystem *)data;
    asys->process(inputBuffer, nBufferFrames);

    return 0;
}

AudioSystem::AudioSystem()
{
    session = std::make_unique<RtAudio>(RtAudio::UNSPECIFIED, &rtaErrorCallback);
    session->showWarnings(true);
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

void AudioSystem::start()
{
    auto &audio = *session;
    std::vector<unsigned int> ids = audio.getDeviceIds();
    auto theInfo = audio.getDeviceInfo(ids[selectedDevice.first]);

    auto sr = theInfo.preferredSampleRate;

    GLOG("Starting audio " << theInfo.name << " at " << sr << " with " << theInfo.inputChannels
                           << " input channels");

    auto audioInputDeviceID = ids[selectedDevice.first];
    auto audioInputUsed = true;
    auto audioOutputUsed = false;

    auto dids = session->getDeviceIds();
    auto dnms = session->getDeviceNames();

    RtAudio::StreamParameters oParams;
    uint32_t sampleRate{sr};

    RtAudio::StreamParameters iParams;
    iParams.deviceId = audioInputDeviceID;
    auto inInfo = session->getDeviceInfo(iParams.deviceId);
    iParams.nChannels = std::min(2U, inInfo.inputChannels);
    nChannels = iParams.nChannels;
    iParams.firstChannel = 0;
    if (sampleRate < 0)
        sampleRate = inInfo.preferredSampleRate;

    if (sampleRate < 0)
    {
        GLOG("No preferred sample rate detected; using 48k");
        sampleRate = 48000;
    }

    RtAudio::StreamOptions options;
    options.flags = RTAUDIO_SCHEDULE_REALTIME;

    uint32_t bufferFrames{256};
    if (session->openStream(nullptr, &iParams, RTAUDIO_FLOAT32, sampleRate, &bufferFrames,
                            &rtaCallback, (void *)this, &options))
    {
        GLOG("[ERROR]" << session->getErrorText());
        session->closeStream();
        return;
    }

    if (!session->isStreamOpen())
    {
        GLOG("[ERROR] Stream failed to open : " << session->getErrorText());
        return;
    }

    if (session->startStream())
    {
        GLOG("[ERROR] startStream failed : " << session->getErrorText());
        return;
    }

    GLOG("RtAudio: Started Stream");
}

void AudioSystem::stop()
{
    if (!isRunning())
        return;
    session->stopStream();
    session->closeStream();
}

bool AudioSystem::isRunning() const { return session->isStreamRunning(); }

void AudioSystem::process(void *inData, int nFrames)
{
    auto *d = (float *)inData;
    // For now only use one channel
    for (int i = 0; i < nFrames * nChannels; i += nChannels)
    {
        auto nv = std::max(std::fabs(d[i]), level.load()) * 0.9995f;
        level = nv;
    }
}
} // namespace audioviz::audio
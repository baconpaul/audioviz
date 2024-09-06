//
// Created by Paul Walker on 9/5/24.
//

#include "devices.h"

#include "RtAudio.h"

#include "infra/glog.h"

namespace audioviz::audio
{
std::vector<std::string> inputDevices()
{
    GLOG("Listing input devices");
    RtAudio audio;
    // Get the list of device IDs
    std::vector<unsigned int> ids = audio.getDeviceIds();
    if (ids.size() == 0)
    {
        GLOG("No devices found.");
        return {};
    }

    // Scan through devices for various capabilities
    RtAudio::DeviceInfo info;
    std::vector<std::string> res;
    for (unsigned int n = 0; n < ids.size(); n++)
    {
        info = audio.getDeviceInfo(ids[n]);
        if (info.inputChannels > 0)
        {
            res.push_back(info.name);
        }
    }
    return res;
}

std::string defaultInputDevice()
{
    RtAudio audio;
    // Get the list of device IDs
    std::vector<unsigned int> ids = audio.getDeviceIds();
    if (ids.size() == 0)
    {
        GLOG("No devices found.");
        return {};
    }

    // Scan through devices for various capabilities
    RtAudio::DeviceInfo info;
    std::vector<std::string> res;
    for (unsigned int n = 0; n < ids.size(); n++)
    {
        info = audio.getDeviceInfo(ids[n]);
       
        if (info.isDefaultInput)
        {
            return info.name;
        }
    }
    return {};
}

void startInput(int idx)
{
    RtAudio audio;
    // Get the list of device IDs
    std::vector<unsigned int> ids = audio.getDeviceIds();
    RtAudio::DeviceInfo info, theInfo;
    int ii{0};
    for (unsigned int n = 0; n < ids.size(); n++)
    {
        info = audio.getDeviceInfo(ids[n]);
        if (info.inputChannels > 0)
        {
            if (ii == idx)
            {
                theInfo = info;
            }
            ii++;
        }
    }
    GLOG("Starting on " << theInfo.name);
}
} // namespace audioviz::audio
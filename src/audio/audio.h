//
// Created by Paul Walker on 9/5/24.
//

#ifndef AUDIOVIZ_AUDIO_H
#define AUDIOVIZ_AUDIO_H

#include <vector>
#include <string>
class RtAudio;

namespace audioviz::audio
{
struct AudioSystem
{
    AudioSystem();
    ~AudioSystem();
    std::vector<std::string> inputDevices();
    std::string defaultInputDevice();
    void startInput(int idx);

  private:
    std::unique_ptr<RtAudio> session;
};
} // namespace audioviz::audio
#endif // AUDIOVIZ_AUDIO_H

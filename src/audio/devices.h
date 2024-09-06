//
// Created by Paul Walker on 9/5/24.
//

#ifndef AUDIOVIZ_DEVICES_H
#define AUDIOVIZ_DEVICES_H

#include <vector>
#include <string>

namespace audioviz::audio
{
std::vector<std::string> inputDevices();
std::string defaultInputDevice();
void startInput(int idx);
} // namespace audioviz::audio
#endif // AUDIOVIZ_DEVICES_H

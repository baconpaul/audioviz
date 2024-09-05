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

#ifndef AUDIOVIZ_SRC_AUDIOVIZ_H
#define AUDIOVIZ_SRC_AUDIOVIZ_H

#include <iostream>

#define GLOG(...)                                                                                  \
    std::cout << __FILE__ << ":" << __LINE__ << " [" << __func__ << "] " << __VA_ARGS__            \
              << std::endl;

#endif // AUDIOVIZ_AUDIOVIZ_H

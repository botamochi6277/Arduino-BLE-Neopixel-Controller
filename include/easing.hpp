#ifndef EASING_HPP
#define EASING_HPP
#include <Arduino.h>
namespace easing
{

    unsigned char linear(int frame, int duration = 100)
    {
        if (frame < 0)
        {
            return 0;
        }
        if (frame >= duration)
        {
            return 255;
        }
        auto v = map(frame, 0, duration, 0, 255);
        return v;
    }
}

#endif
#ifndef EASING_HPP
#define EASING_HPP
#include <Arduino.h>

// https://easings.net
namespace easing
{

    const short LINEAR = 0;
    const short SINE = 1;

    template <typename T>
    T remap(T x, T in_min, T in_max, T out_min, T out_max)
    {
        return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
    }
    /**
     * @brief linear easing
     *
     * @param frame
     * @param duration
     * @return int 0--255
     */
    int linear(int frame, int duration = 255)
    {
        if (frame < 0)
        {
            return 0;
        }
        if (frame >= duration)
        {
            return 255;
        }
        auto v = remap(frame, 0, duration, 0, 255);
        return v;
    }

    int inOutSine(int frame, int duration = 255)
    {
        if (frame < 0)
        {
            return 0;
        }
        if (frame >= duration)
        {
            return 255;
        }
        float v = sin(frame / duration * PI - 0.5f * PI); // [-1,1]
        return remap(v, -1.0f, 1.0f, 0.0f, 255.0f);
    }

    int inSine(int frame, int duration = 255)
    {
        if (frame < 0)
        {
            return 0;
        }
        if (frame >= duration)
        {
            return 255;
        }
        float v = sin(frame / duration * 0.5f * PI - 0.5f * PI); // [-1,0]

        return remap(v, -1.0f, 0.0f, 0.0f, 255.0f);
    }

    int outSine(int frame, int duration = 255)
    {
        if (frame < 0)
        {
            return 0;
        }
        if (frame >= duration)
        {
            return 255;
        }
        float v = sin(frame / duration * 0.5f * PI); //[0,1]
        return remap(v, 0.0f, 1.0f, 0.0f, 255.0f);
    }

}

#endif
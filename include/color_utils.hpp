
#ifndef COLOR_UTILS_HPP
#define COLOR_UTILS_HPP
#include <Arduino.h>
#include "easing.hpp"

void colorToInt(unsigned int c, unsigned char &r, unsigned char &g, unsigned char &b, unsigned char &w)
{
    w = c >> 24;
    r = c >> 16;
    g = c >> 8;
    b = c;
}

unsigned int colorEasing(
    unsigned int from, unsigned int to,
    int frame, int duration, short ease = easing::LINEAR)
{

    unsigned char rf, gf, bf, wf;
    colorToInt(from, rf, gf, bf, wf);
    unsigned char rt, gt, bt, wt;
    colorToInt(to, rt, gt, bt, wt);

    unsigned int color;
    unsigned char ri, gi, bi, wi;

    unsigned char w = 0; // weight, 0--255
    switch (ease)
    {
    case easing::LINEAR:
        w = easing::linear(frame, duration);
        break;
    case easing::SINE:
        w = easing::inOutSine(frame, duration);
        break;

    default:
        break;
    }

    ri = easing::remap((unsigned char)w, (unsigned char)0, (unsigned char)255, rf, rt);
    gi = easing::remap((unsigned char)w, (unsigned char)0, (unsigned char)255, gf, gt);
    bi = easing::remap((unsigned char)w, (unsigned char)0, (unsigned char)255, bf, bt);
    color = ri << 16 | gi << 8 | bi;
    return color;
}

void wipeUpdate(
    unsigned int colors[], unsigned int from[], unsigned int to[],
    unsigned int num_pixels, unsigned int progress, bool forward = true)
{
    // unsigned int num_seq = num_pixels + 1;
    for (size_t i = 0; i < num_pixels; i++)
    {
        if (progress < i)
        {
            colors[i] = from[i];
        }
        else
        {
            colors[i] = to[i];
        }
    }
}

void dissolveUpdate(unsigned int colors[], unsigned int from[], unsigned int to[],
                    unsigned int num_pixels, unsigned int progress,
                    unsigned int duration)
{
    // float w = progress / num_pixels;
    for (size_t i = 0; i < num_pixels; i++)
    {
        // unsigned unsigned int r = w * (to[i] - from[i]) + from[i];
        colors[i] = colorEasing(
            from[i], to[i], progress, duration);
    }
}

void slideUpdate(unsigned int colors[], unsigned int from[], unsigned int to[],
                 unsigned int num_pixels, unsigned int progress)
{
    for (size_t i = 0; i < num_pixels; i++)
    {
        if (progress <= i)
        {
            colors[i] = from[i];
        }
        else
        {
            unsigned int j = num_pixels - progress + i;
            if (num_pixels <= j)
            {
                j = num_pixels - 1;
            }

            colors[i] = to[num_pixels - progress + i];
        }
    }
}

void wipe_dissolve(
    unsigned int colors[], unsigned int from[], unsigned int to[],
    unsigned int num_pixels, unsigned int progress, unsigned int num_dissolve = 3)
{

    // num_seq = num_pixels + num_dissolve
    for (size_t i = 0; i < num_pixels; i++)
    {
        if (progress < i + 1)
        {
            colors[i] = from[i];
        }
        else
        {
            int k = progress - i;
            int w = 255 * k / num_dissolve;

            if (w >= 255)
            {
                colors[i] = to[i];
            }
            else
            {
                colors[i] = colorEasing(from[i], to[i], w, num_dissolve);
            }
        }
    }
}

void slide_dissolve(
    unsigned int colors[], unsigned int from[], unsigned int to[],
    unsigned int num_pixels, unsigned int progress, unsigned int num_dissolve = 3)
{
    for (size_t i = 0; i < num_pixels; i++)
    {
        if (progress <= i)
        {
            colors[i] = from[i];
        }
        else if (num_dissolve + i <= progress)
        {
            size_t k = num_pixels - progress + i + num_dissolve - 1;
            colors[i] = to[k];
        }
        else
        {
            int k = progress - i;
            int w = 255 * k / num_dissolve;

            colors[i] = colorEasing(from[i], to[num_pixels - 1], progress, num_dissolve);
        }
    }
}

void gradient_color(
    unsigned int colors[], size_t num_pixels,
    unsigned int left, unsigned int right)
{
    for (size_t i = 0; i < num_pixels; i++)
    {
        colors[i] = colorEasing(left, right, i, num_pixels);
    }
}

#endif
// Shape functions
#ifndef BOTAMOCHI_SHAPES_HPP
#define BOTAMOCHI_SHAPES_HPP
#include <Arduino.h>

#include "Easing.hpp"
namespace shape {

// intensity function ID
enum class IntensityFuncId : unsigned char {
    kHeat,
    kWipe,
    kPulse,
    // TravelingWave,
    // TravelingPulse,
    // StationaryWave,
    kSineWave,
    kSawWave,
    LENGTH
};

String intensity_func_name(IntensityFuncId id) {
    static String names[] = {"Heat", "Wipe", "Pulse", "SineWave", "SawWave"};
    return names[static_cast<uint8_t>(id)];
}

void heat(float t, float intensity[], uint16_t num_pixels) {
    for (size_t i = 0; i < num_pixels; i++) {
        intensity[i] = t;
    }
}

void wipeQuad(float t, float intensity[], uint16_t num_pixels,
              float blur_width = 0.25f) {
    for (uint16_t index = 0; index < num_pixels; index++) {
        auto x = static_cast<float>(index) / num_pixels;
        auto tt = t - (x / (1.0f + 2.0f * blur_width));

        intensity[index] = easing::quadInOut(tt, blur_width);
    }
}

void pulseQuad(float t, float intensity[], uint16_t num_pixels,
               float pulse_width = 0.25f) {
    // pulse_width 0--1.0 relative length to pixels length
    auto a =
        easing::remap((1.0f - abs(t)), 0.0f, 1.0f, -1.0f * 0.5f * pulse_width,
                      1.0f + 0.5f * pulse_width);
    float v = 1.0f + 3.5f * pulse_width;

    for (uint16_t index = 0; index < num_pixels; index++) {
        auto x = static_cast<float>(index) / num_pixels;
        float tt = (t - pulse_width) - x / v;
        intensity[index] =
            (easing::quadInOut(tt + 0.5f * pulse_width, 0.5f * pulse_width) -
             easing::quadInOut(tt - 0.5f * pulse_width, 0.5f * pulse_width));
    }
}

void waveSaw(float t, float intensity[], uint16_t num_pixels) {
    static float v = 1.001f;

    for (size_t i = 0; i < num_pixels; i++) {
        auto x = (static_cast<float>(i) / num_pixels);

        intensity[i] = ceilf(t - x / v) - (t - x / v);
    }
}

void waveSine(float t, float intensity[], uint16_t num_pixels) {
    for (size_t i = 0; i < num_pixels; i++) {
        auto x = (static_cast<float>(i) / num_pixels);

        intensity[i] = (0.5f * (sinf(2.0f * M_PI * 1.0f * (t - x))) + 0.5f);
    }
}

void setIntensity(float t, float intensity[], uint16_t num_pixels,
                  IntensityFuncId func_id, bool is_reversed = false) {
    if (is_reversed) {
        t = 1.0f - t;
    }

    switch (func_id) {
        case IntensityFuncId::kHeat:
            shape::heat(t, intensity, num_pixels);
            break;
        case IntensityFuncId::kWipe:
            shape::wipeQuad(t, intensity, num_pixels);
            break;
        case IntensityFuncId::kPulse:
            shape::pulseQuad(t, intensity, num_pixels);
            break;
        case IntensityFuncId::kSineWave:
            shape::waveSine(t, intensity, num_pixels);
            break;
        case IntensityFuncId::kSawWave:
            shape::waveSaw(t, intensity, num_pixels);
            break;

        default:
            break;
    }
}
}  // namespace shape

#endif
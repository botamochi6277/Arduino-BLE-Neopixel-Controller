// Shape functions
#ifndef BOTAMOCHI_SHAPES_HPP
#define BOTAMOCHI_SHAPES_HPP
#include <Arduino.h>

#include "Easing.hpp"
namespace shape {

// intensity function ID
enum class IntensityFuncId : unsigned char {
    Heat,
    Wipe,
    Pulse,
    // TravelingWave,
    // TravelingPulse,
    // StationaryWave,
    SineWave,
    SawWave,
    LENGTH
};

String intensity_func_name(IntensityFuncId id) {
    static String names[] = {"Heat", "Wipe", "Pulse", "SineWave", "SawWave"};
    return names[static_cast<uint8_t>(id)];
}

void heat(float intensity[], uint16_t num_pixels, float magnitude) {
    for (size_t i = 0; i < num_pixels; i++) {
        intensity[i] = magnitude;
    }
}

void wipeQuad(float intensity[], uint16_t num_pixels, float magnitude,
              float blur_width = 0.25f, bool is_reversed = false) {
    // 1.0f or -1.0f
    auto sign = abs(magnitude) > 1.0e-3f ? magnitude / abs(magnitude) : 1.0f;
    // blur_width 0--1.0 relative length to pixels length
    auto a = easing::remap((1.0f - abs(magnitude)), 0.0f, 1.0f,
                           -1.0f * blur_width, 1.0f);

    for (uint16_t index = 0; index < num_pixels; index++) {
        auto x = static_cast<float>(index) / num_pixels;
        if (is_reversed) {
            x = 1.0f - x;
        }

        intensity[index] = sign * easing::quadInOut(x - a, blur_width);
    }
}

void pulseQuad(float intensity[], uint16_t num_pixels, float magnitude,
               float pulse_width = 0.25f, bool is_reversed = false) {
    auto sign = abs(magnitude) > 1.0e-3f ? magnitude / abs(magnitude) : 1.0f;
    // pulse_width 0--1.0 relative length to pixels length
    auto a =
        easing::remap((1.0f - abs(magnitude)), 0.0f, 1.0f,
                      -1.0f * 0.5f * pulse_width, 1.0f + 0.5f * pulse_width);

    for (uint16_t index = 0; index < num_pixels; index++) {
        auto x = static_cast<float>(index) / num_pixels;
        if (is_reversed) {
            x = 1.0f - x;
        }
        intensity[index] =
            sign *
            (easing::quadInOut(x + 0.5f * pulse_width - a, 0.5f * pulse_width) -
             easing::quadInOut(x - 0.5f * pulse_width - a, 0.5f * pulse_width));
    }
}

void waveSaw(float intensity[], uint16_t num_pixels, float magnitude,
             float wave_width = 1.0f, bool is_reversed = false) {
    auto a = 1.0f - magnitude;

    for (size_t i = 0; i < num_pixels; i++) {
        auto x = (static_cast<float>(i) / num_pixels) / (wave_width + 1e-9f);
        if (is_reversed) {
            x = 1.0f - x;
        }
        intensity[i] = ((x - a) - floorf((x - a) + 0.5f) + 0.5f);
    }
}

void waveSine(float intensity[], uint16_t num_pixels, float magnitude,
              float wave_width = 1.0f, float is_reversed = false) {
    auto a = 1.0f - magnitude;

    for (size_t i = 0; i < num_pixels; i++) {
        auto x = (static_cast<float>(i) / num_pixels) / (wave_width + 1e-9f);
        if (is_reversed) {
            x = 1.0f - x;
        }
        intensity[i] = (0.5f * (sinf(2.0f * M_PI * 1.0f * (x - a))) + 0.5f);
    }
}

void setIntensity(float intensity[], uint16_t num_pixels, float magnitude,
                  IntensityFuncId func_id, float width = 0.25f,
                  bool is_reversed = false) {
    switch (func_id) {
        case IntensityFuncId::Heat:
            shape::heat(intensity, num_pixels, magnitude);
            break;
        case IntensityFuncId::Wipe:
            shape::wipeQuad(intensity, num_pixels, magnitude, width,
                            is_reversed);
            break;
        case IntensityFuncId::Pulse:
            shape::pulseQuad(intensity, num_pixels, magnitude, width,
                             is_reversed);
            break;
        case IntensityFuncId::SineWave:
            shape::waveSine(intensity, num_pixels, magnitude, width,
                            is_reversed);
            break;
        case IntensityFuncId::SawWave:
            shape::waveSaw(intensity, num_pixels, magnitude, width,
                           is_reversed);
            break;

        default:
            break;
    }
}
}  // namespace shape

#endif
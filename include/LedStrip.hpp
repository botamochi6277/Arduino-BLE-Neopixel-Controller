#ifndef STRIP_HPP
#define STRIP_HPP
#include <Adafruit_NeoPixel.h>
#include <Arduino.h>

#ifndef NUM_PIXELS
#define NUM_PIXELS 45
#endif
#include "ColorUtils.hpp"
#include "Colormap.hpp"
#include "PixelUnit.hpp"
#include "Shapes.hpp"

namespace led_strip {

void fill(uint32_t hsb_colors[], uint16_t num_pixels, uint32_t color) {
    for (uint16_t i = 0; i < num_pixels; i++) {
        hsb_colors[i] = color;
    }
}

void blend(uint32_t hsb_colors[], uint32_t palette_hsb[], uint16_t num_pixels,
           uint8_t palette_size = 2, uint8_t blend_type = 0) {
    if (palette_size == 0) {
        return;
    }

    if (palette_size == 1) {
        for (size_t i = 0; i < num_pixels; i++) {
            hsb_colors[i] = palette_hsb[0];
        }
        return;
    }

    // blend type
    // 0: gradient
    // 1: cycle
    switch (blend_type) {
        case 0:
            // gradient
            for (uint8_t i = 0; i < num_pixels; i++) {
                hsb_colors[i] =
                    color::hsbhexEasing(palette_hsb[0], palette_hsb[1],
                                        static_cast<float>(i) / num_pixels);
            }

            break;

        default:
            break;
    }
}

void easeColors(uint32_t hsb_colors[], uint32_t hsb_from[], uint32_t hsb_to[],
                float weights[], uint16_t num_pixels) {
    for (uint16_t i = 0; i < num_pixels; i++) {
        hsb_colors[i] = color::hsbhexEasing(hsb_from[i], hsb_to[i], weights[i]);
    }
}

// transition
void dissolveWeight(float weights[], uint16_t num_pixels,
                    float progress_ratio) {
    for (size_t i = 0; i < num_pixels; i++) {
        weights[i] = progress_ratio;
    }
}

void wipeWeight(float weights[], uint16_t num_pixels, float progress_ratio,
                float blur_width, bool is_backward = false) {
    // blur_width 0--1.0 relative length to pixels length
    for (uint16_t index = 0; index < num_pixels; index++) {
        auto r = static_cast<float>(index) / num_pixels;
        if (!is_backward) {
            weights[index] = easing::hardSigmoidInOut(
                (1.0f - r) + (1.0f + blur_width) * progress_ratio - 1.0f,
                blur_width);
        } else {
            weights[index] = easing::hardSigmoidInOut(
                r + (1.0f + blur_width) * progress_ratio - 1.0f, blur_width);
        }
    }
};

bool dissolveEasing(uint32_t hsb_colors[], uint32_t hsb_from[],
                    uint32_t hsb_to[], float weights[], uint16_t num_pixels,
                    float progress) {
    dissolveWeight(weights, num_pixels, progress);
    easeColors(hsb_colors, hsb_from, hsb_to, weights, num_pixels);
    return progress >= 1.0f;
}

bool wipeEasing(uint32_t hsb_colors[], uint32_t hsb_from[], uint32_t hsb_to[],
                float weights[], uint16_t num_pixels, float progress,
                bool is_backward = false, uint16_t blur_length = 3U) {
    float blur_width = static_cast<float>(blur_length) / num_pixels;
    wipeWeight(weights, num_pixels, progress, blur_width, is_backward);
    easeColors(hsb_colors, hsb_from, hsb_to, weights, num_pixels);
    return progress >= 1.0f;
}

bool slideEasing(uint32_t hsb_colors[], uint32_t hsb_from[], uint32_t hsb_to[],
                 float weights[], uint16_t num_pixels, float progress,
                 bool is_backward = false, uint16_t blur_length = 3U) {
    float blur_width = static_cast<float>(blur_length) / num_pixels;
    wipeWeight(weights, num_pixels, progress, blur_width, is_backward);
    // use hsb_colors as shifted color cache
    //  this is shallow copy
    auto shifted_goal_colors = hsb_colors;

    uint16_t neck_idx = 0U;
    if (!is_backward) {
        uint16_t reversed_idx = num_pixels - 1;
        for (uint16_t index = 0; index < num_pixels; index++) {
            reversed_idx = num_pixels - 1 - index;
            if (weights[index] < 0.99) {
                shifted_goal_colors[reversed_idx] = hsb_to[num_pixels - 1];
            } else {
                neck_idx = index;
                break;
            }
        }
        for (uint16_t index = neck_idx; index < num_pixels; index++) {
            reversed_idx = num_pixels - 1 - index;
            shifted_goal_colors[index] =
                hsb_to[num_pixels - 1 + neck_idx - index];
        }
    } else {
        // backward
        for (uint16_t index = 0; index < num_pixels; index++) {
            if (weights[index] < 0.99) {
                shifted_goal_colors[index] = hsb_to[0];
            } else {
                neck_idx = index;
                break;
            }
        }
        for (uint16_t index = neck_idx; index < num_pixels; index++) {
            shifted_goal_colors[index] = hsb_to[index];
        }
    }

    easeColors(hsb_colors, hsb_from, shifted_goal_colors, weights, num_pixels);

    return progress > 1.0f;
}

// intensity functions
enum class IntensityFuncId : unsigned char {
    Heat,  // dissolve
    Wipe,
    Pulse,
    TravelingWave,
    // TravelingPulse,
    StationaryWave,
    Cycle,
    Shift,
    LENGTH
};

String intensity_func_name(IntensityFuncId id) {
    static String names[] = {"Heat",          "Wipe",           "Pulse",
                             "TravelingWave", "StationaryWave", "Cycle",
                             "Shift"};
    return names[static_cast<uint8_t>(id)];
}

float travelingWave(float freq, float time, float position, float speed,
                    float initial_phase) {
    return 0.5f * sinf(2.0f * M_PI * freq *
                       (time - (position / (speed + 1e-9f)))) +
           0.5f;
}

// pulse function
// float travelingPulse(float time, float position, float speed, float width) {
//   return easing::trianglePulse(position + time * speed, width);
// }

class PixelManager {
   private:
    colormap::ColormapId cmap_;

    float transition_start_sec_;
    float intensity_[NUM_PIXELS];  // separate from this class?

    IntensityFuncId func_id_;
    //  params to compute intensity
    float wave_width_;  // the wave width of wipe's blur or pulse wave

    // legecy
    float wave_freq_;   // wave freq of traveling_wave, cycle
    float wave_speed_;  // speed of wave

   public:
    PixelManager();
    PixelManager(float pitch);
    color::PixelUnit pixel_units[NUM_PIXELS];

    colormap::ColormapId colormap();
    void setColormap(colormap::ColormapId id);
    void setColormap(uint8_t id);

    inline IntensityFuncId intensity_func_id() { return this->func_id_; };
    void setIntensityFuncId(IntensityFuncId id);
    void setIntensityFuncId(uint8_t id);

    void setWaveWidth(float width);
    void setWaveWidth(uint8_t width, float min, float max);

    void setWaveFreq(float freq);
    void setWaveFreq(uint8_t freq, float min, float max);
    inline float waveFreq() { return this->wave_freq_; }

    void setWaveSpeed(float speed);
    void setWaveSpeed(uint8_t speed, float min, float max);

    void setIntensity(float intensity[]);
    void computeAndSetIntensity(float magnitude);

    void setColor();  // set color with intensity[]
    void setColor(colormap::ColormapId cmap);
};

PixelManager::PixelManager()
    : PixelManager(1.0f / static_cast<float>(NUM_PIXELS - 1)) {}

PixelManager::PixelManager(float pitch) {
    for (size_t i = 0; i < NUM_PIXELS; i++) {
        this->pixel_units[i].setPosition(i * pitch);
        this->intensity_[i] = 0.0f;
    }
    this->wave_width_ = 0.1f;
    this->wave_freq_ = 0.5f;
    this->wave_speed_ = 0.1f;
}

colormap::ColormapId PixelManager::colormap() { return this->cmap_; };
void PixelManager::setColormap(colormap::ColormapId id) { this->cmap_ = id; }
void PixelManager::setColormap(uint8_t id) {
    this->setColormap(static_cast<colormap::ColormapId>(id));
}
void PixelManager::setIntensityFuncId(IntensityFuncId id) {
    this->func_id_ = id;
}
void PixelManager::setIntensityFuncId(uint8_t id) {
    this->setIntensityFuncId(static_cast<IntensityFuncId>(id));
}

void PixelManager::setWaveWidth(float width) { this->wave_width_ = width; }
void PixelManager::setWaveWidth(uint8_t width, float min, float max) {
    this->wave_width_ =
        easing::remap(static_cast<float>(width), 0.0f, 255.0f, min, max);
}

void PixelManager::setWaveFreq(float freq) { this->wave_freq_ = freq; }
void PixelManager::setWaveFreq(uint8_t freq, float min, float max) {
    this->wave_freq_ =
        easing::remap(static_cast<float>(freq), 0.0f, 255.0f, min, max);
}

void PixelManager::setWaveSpeed(float speed) { this->wave_speed_ = speed; }
void PixelManager::setWaveSpeed(uint8_t speed, float min, float max) {
    this->wave_speed_ =
        easing::remap(static_cast<float>(speed), 0.0f, 255.0f, min, max);
}

void PixelManager::setIntensity(float intensity[]) {
    for (size_t i = 0; i < NUM_PIXELS; i++) {
        this->intensity_[i] = intensity[i];
    }
}

void PixelManager::computeAndSetIntensity(float magnitude) {
    switch (this->func_id_) {
        case IntensityFuncId::Heat:
            shape::heat(this->intensity_, NUM_PIXELS, magnitude);
            break;
        case IntensityFuncId::Wipe:
            shape::wipeQuad(this->intensity_, NUM_PIXELS, magnitude,
                            this->wave_width_, (this->wave_speed_ < 0.0f));
            break;
        case IntensityFuncId::Pulse:
            for (size_t i = 0; i < NUM_PIXELS; i++) {
                this->intensity_[i] = easing::trianglePulse(
                    this->pixel_units[i].position() - magnitude,
                    this->wave_width_);
            }
            break;
        case IntensityFuncId::TravelingWave:
            for (size_t i = 0; i < NUM_PIXELS; i++) {
                this->intensity_[i] = travelingWave(
                    this->wave_freq_, magnitude,
                    this->pixel_units[i].position(), this->wave_speed_, 0.0f);
            }
            break;
        case IntensityFuncId::Cycle:
            for (size_t i = 0; i < NUM_PIXELS; i++) {
                this->intensity_[i] =
                    0.5f * sinf(2.0f * M_PI * this->wave_freq_ * magnitude) +
                    0.5f;
            }
            break;
        case IntensityFuncId::Shift:
            for (size_t i = 0; i < NUM_PIXELS; i++) {
                float tmp = magnitude + ((float)i / NUM_PIXELS);
                if (tmp < 1.0f) {
                    this->intensity_[i] = tmp;
                } else {
                    this->intensity_[i] = tmp - 1.0f;
                }
            }
            break;

        default:
            break;
    }
}

void PixelManager::setColor() {
    for (size_t i = 0; i < NUM_PIXELS; i++) {
        this->pixel_units[i].setCmapColor(this->intensity_[i], this->cmap_);
    }
}

void PixelManager::setColor(colormap::ColormapId cmap) {
    this->cmap_ = cmap;
    this->setColor();
}

};  // namespace led_strip

#endif
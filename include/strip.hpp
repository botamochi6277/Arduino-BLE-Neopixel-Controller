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
        hsb_colors[i] = color::hsbhexEasing(palette_hsb[0], palette_hsb[1],
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
      weights[index] = easing::hardSigmoid(
          (1.0f - r) + (1.0f + blur_width) * progress_ratio - 1.0f, blur_width);
    } else {
      weights[index] = easing::hardSigmoid(
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
      shifted_goal_colors[index] = hsb_to[num_pixels - 1 + neck_idx - index];
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
  TravelingWave,
  TravelingPulse
};
float travelingWave(float freq, float time, float position, float speed,
                    float initial_phase) {
  return 0.5f *
         sinf(2.0f * M_PI * freq * (time - (position / (speed + 1e-9f))) +
              0.5f);
}

// pulse function

class PixelManager {
 private:
  //  anchor points
  uint32_t current_colors[NUM_PIXELS];  // new color buffer
  uint32_t start_colors[NUM_PIXELS];
  uint32_t transited_colors[NUM_PIXELS];
  uint32_t cache_colors[NUM_PIXELS];
  uint32_t palette[4];
  float transition_weights[NUM_PIXELS];
  uint32_t fluctuation_colors[NUM_PIXELS];

  float transition_start_sec_;
  float intensity_[NUM_PIXELS];

 public:
  PixelManager();
  PixelManager(float pitch);
  color::PixelUnit pixel_units[NUM_PIXELS];

  void setPaletteColor(uint8_t index, unsigned int color);
  unsigned int getPaletteColor(uint8_t index);

  void setCurrentColor(uint8_t index, unsigned int color);
  void setFluctuationColor(uint8_t index, unsigned int color);
  void blend(uint32_t hsb_colors[], uint32_t palette_hsb[], uint16_t num_pixels,
             uint8_t palette_size = 2, uint8_t blend_type = 0);

  void setIntensity(float intensity[]);
  void setIntensity(float value, IntensityFuncId func_id);
  void setColor();  // set color with intensity[]
  void setColor(colormap::ColormapId cmap);
};

PixelManager::PixelManager() {
  for (size_t i = 0; i < NUM_PIXELS; i++) {
    this->pixel_units[i].setPosition(i * 1.0f / (NUM_PIXELS - 1));
  }
}

PixelManager::PixelManager(float pitch) {
  for (size_t i = 0; i < NUM_PIXELS; i++) {
    this->pixel_units[i].setPosition(i * pitch);
  }
}

void PixelManager::setPaletteColor(uint8_t index, unsigned int color) {
  this->palette[index] = color;
}

unsigned int PixelManager::getPaletteColor(uint8_t index) {
  return this->palette[index];
}

void PixelManager::setCurrentColor(uint8_t index, unsigned int color) {
  this->current_colors[index] = color;
}
void PixelManager::setFluctuationColor(uint8_t index, unsigned int color) {
  this->fluctuation_colors[index] = color;
}

void PixelManager::setIntensity(float intensity[]) {
  for (size_t i = 0; i < NUM_PIXELS; i++) {
    this->intensity_[i] = intensity[i];
  }
}

void PixelManager::setIntensity(float value, IntensityFuncId func_id) {
  switch (func_id) {
    case IntensityFuncId::Heat:
      dissolveWeight(this->intensity_, NUM_PIXELS, value);
      break;
    case IntensityFuncId::Wipe:
      wipeWeight(this->intensity_, NUM_PIXELS, value, 0.1);
      break;
    case IntensityFuncId::TravelingWave:
      for (size_t i = 0; i < NUM_PIXELS; i++) {
        travelingWave(1.0f, value, this->pixel_units[i].position(), 1.0f, 0.0f);
      }
      break;
    default:
      break;
  }
}
void PixelManager::setColor(colormap::ColormapId cmap) {
  for (size_t i = 0; i < NUM_PIXELS; i++) {
    this->pixel_units[i].setCmapColor(this->intensity_[i], cmap);
  }
}

};  // namespace led_strip

#endif
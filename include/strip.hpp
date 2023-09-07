#ifndef STRIP_HPP
#define STRIP_HPP
#include <Adafruit_NeoPixel.h>
#include <Arduino.h>

#include "color_utils.hpp"

namespace led_strip {
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

// /**
//  * @brief assign easing color
//  *
//  * @param colors array of colors
//  * @param num_pixels
//  * @param left
//  * @param right
//  */
// void gradientColor(uint32_t colors[], size_t num_pixels, uint32_t left,
//                    uint32_t right) {
//   for (size_t i = 0; i < num_pixels; i++) {
//     colors[i] = color::rgbEasing(left, right, static_cast<float>(i) /
//     num_pixels);
//   }
// }

// void rainbowColor(uint32_t colors[], size_t num_pixels, unsigned char s =
// 255,
//                   unsigned char v = 200) {
//   unsigned char hue = 0;

//   for (size_t i = 0; i < num_pixels; i++) {
//     hue = (0xFFFF * i) / num_pixels;
//     colors[i] = color::hsbToHex(hue, s, v);
//   }
// }
};  // namespace led_strip

#endif
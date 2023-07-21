
#ifndef COLOR_UTILS_HPP
#define COLOR_UTILS_HPP

#include <math.h>
// typedef unsigned char uint8_t;
// typedef uint32_t uint32_t;
#include "easing.hpp"

namespace color {

void hexToRgbw(uint32_t color, uint8_t &r, uint8_t &g, uint8_t &b, uint8_t &w) {
  // 0x FF 00 FF 00
  w = (0xFF000000 & color) >> 24;
  r = (0x00FF0000 & color) >> 16;
  g = (0x0000FF00 & color) >> 8;
  b = (0x000000FF & color);
}

void hexToRgb(uint32_t color, uint8_t &r, uint8_t &g, uint8_t &b) {
  // 0x FF 00 FF 00
  r = (0x00FF0000 & color) >> 16;
  g = (0x0000FF00 & color) >> 8;
  b = (0x000000FF & color);
}

uint32_t hsbToHex(uint16_t hue, uint8_t saturation, uint8_t brightness) {
  uint8_t r = 0, g = 0, b = 0;

  // 10922.5 = 65535/6 , approx 10922
  //   hi : 0--5
  uint8_t hi = static_cast<uint16_t>(static_cast<float>(hue) / 10922.5f) % 6;

  float f = hue / (10922.5f) - hi;
  uint8_t p = brightness * (1 - saturation / 255);
  uint8_t q = brightness * (1 - (f * saturation) / 255);
  uint8_t t = brightness * (1 - ((1 - f) * saturation) / 255);

  switch (hi) {
    case 0:
      r = brightness;
      g = t;
      b = p;
      break;
    case 1:
      r = q;
      g = brightness;
      b = p;
      break;
    case 2:
      r = p;
      g = brightness;
      b = t;
      break;
    case 3:
      r = p;
      g = q;
      b = brightness;
      break;
    case 4:
      r = t;
      g = p;
      b = brightness;
      break;
    case 5:
      r = brightness;
      g = p;
      b = q;
      break;
  }

  return (r << 16 | g << 8 | b);
}

void rgbToHsb(uint8_t r, uint8_t g, uint8_t b, uint16_t &hue,
              uint8_t &saturation, uint8_t &brightness) {
  float fr = r / 255.0f;
  float fg = g / 255.0f;
  float fb = b / 255.0f;

  float mi = std::fmin(std::fmin(fr, fg), fb);
  float ma = std::fmax(std::fmax(fr, fg), fb);
  float delta = ma - mi;

  brightness = static_cast<uint16_t>(ma * 65535);

  if (std::fabs(delta) < 1e-6) {
    // this is gray
    saturation = 0;
    return;
  }

  saturation = static_cast<uint8_t>(255 * (delta / ma));

  float delta_r = (((ma - fr) * 0.16666666666f) + (delta * 0.5f)) / delta;
  float delta_g = (((ma - fg) * 0.16666666666f) + (delta * 0.5f)) / delta;
  float delta_b = (((ma - fb) * 0.16666666666f) + (delta * 0.5f)) / delta;

  float f_hue = 0.0f;
  if (std::fabs(fr - ma) < 1e-6) {
    f_hue = delta_b - delta_g;
  } else if (std::fabs(fg - ma) < 1e-6) {
    f_hue = 0.3333333333f + delta_r - delta_b;
  } else if (std::fabs(fb - ma) < 1e-6) {
    f_hue = 0.6666666666f + delta_g - delta_r;
  }

  if (f_hue < 0.0f) {
    f_hue += 1.0;
  }
  if (f_hue > 1.0f) {
    f_hue -= 1.0;
  }

  hue = static_cast<uint8_t>(255 * f_hue);
}

void hexToHsb(uint32_t color, uint16_t &hue, uint8_t &saturation,
              uint8_t &brightness) {
  uint8_t r, g, b;
  hexToRgb(color, r, g, b);
  rgbToHsb(r, g, b, hue, saturation, brightness);
}

uint32_t rgbEasing(uint32_t from, uint32_t to, int frame, int duration,
                   short ease = easing::LINEAR) {
  unsigned char rf, gf, bf, wf;
  hexToRgbw(from, rf, gf, bf, wf);
  unsigned char rt, gt, bt, wt;
  hexToRgbw(to, rt, gt, bt, wt);

  uint32_t color;
  unsigned char ri, gi, bi;

  unsigned char w = 0;  // weight, 0--255
  switch (ease) {
    case easing::LINEAR:
      w = easing::linear(frame, duration);
      break;
    case easing::SINE:
      w = easing::inOutSine(frame, duration);
      break;

    default:
      break;
  }

  ri = easing::remap((unsigned char)w, (unsigned char)0, (unsigned char)255, rf,
                     rt);
  gi = easing::remap((unsigned char)w, (unsigned char)0, (unsigned char)255, gf,
                     gt);
  bi = easing::remap((unsigned char)w, (unsigned char)0, (unsigned char)255, bf,
                     bt);
  color = ri << 16 | gi << 8 | bi;
  return color;
}

uint32_t hsbEasing(uint32_t from, uint32_t to, uint16_t frame,
                   uint16_t duration, uint8_t ease = 0) {
  uint16_t h0, h1;
  uint8_t s0, s1, b0, b1;

  hexToHsb(from, h0, s0, b0);
  hexToHsb(to, h1, s1, b1);

  uint16_t w = 0;  // weight
  switch (ease) {
    case easing::LINEAR:
      w = easing::linear(frame, duration);
      break;
    case easing::SINE:
      w = easing::inOutSine(frame, duration);
      break;

    default:
      break;
  }

  uint16_t h =
      easing::remap((uint16_t)w, (uint16_t)0, (uint16_t)0xFFFF, h0, h1);
  uint8_t s =
      easing::remap((uint8_t)(w / 255), (uint8_t)0, (uint8_t)0xFF, s0, s1);
  uint8_t b =
      easing::remap((uint8_t)(w / 255), (uint8_t)0, (uint8_t)0xFF, b0, b1);

  return hsbToHex(h, s, b);
}
void blend(uint32_t colors[], uint32_t palette[], uint8_t num_pixels,
           uint8_t palette_size = 2, uint8_t blend_type = 0) {
  if (palette_size == 0) {
    return;
  }

  if (palette_size == 1) {
    for (size_t i = 0; i < num_pixels; i++) {
      colors[i] = palette[0];
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
        colors[i] = hsbEasing(palette[0], palette[1], i, num_pixels);
      }

      break;

    default:
      break;
  }
}

bool wipeUpdate(uint32_t colors[], uint32_t from[], uint32_t to[],
                uint32_t num_pixels, uint32_t progress, bool forward = true) {
  bool b;
  for (size_t i = 0; i < num_pixels; i++) {
    if (forward) {
      b = (size_t)progress - 1 < i;
    } else {
      b = (num_pixels - progress - 1) < i;
    }

    if (b) {
      colors[i] = from[i];
    } else {
      colors[i] = to[i];
    }
  }

  return progress >= num_pixels - 1;
}

bool dissolveUpdate(uint32_t colors[], uint32_t from[], uint32_t to[],
                    uint32_t num_pixels, uint32_t progress, uint32_t duration) {
  // float w = progress / num_pixels;
  for (uint32_t i = 0; i < num_pixels; i++) {
    colors[i] = hsbEasing(from[i], to[i], progress, duration, 0);
  }
  return progress >= duration;
}

bool slideUpdate(uint32_t colors[], uint32_t from[], uint32_t to[],
                 uint32_t num_pixels, uint32_t progress, bool forward = true) {
  uint32_t head;
  uint32_t tail;
  bool b;
  for (size_t i = 0; i < num_pixels; i++) {
    head = progress - 1;
    tail = head - num_pixels + 1;
    if (forward) {
      b = head < i;
    } else {
      b = i < tail;
    }

    if (b) {
      colors[i] = from[i];
    } else {
      if (forward) {
        colors[i] = to[num_pixels - (head - i) - 1];
      } else {
        colors[i] = to[i - tail];
      }
    }
  }

  return progress >= num_pixels;
}

bool wipe_dissolve(uint32_t colors[], uint32_t from[], uint32_t to[],
                   uint32_t num_pixels, uint32_t progress,
                   uint32_t num_dissolve = 3, bool forward = true) {
  bool b;
  // num_seq = num_pixels + num_dissolve
  for (size_t i = 0; i < num_pixels; i++) {
    if (forward) {
      b = progress < i + 1;
    } else {
      b = num_pixels - progress - 1;
    }

    if (b) {
      colors[i] = from[i];
    } else {
      int k = 0;
      if (forward) {
        k = progress - i;
      } else {
        k = i - (num_pixels - progress - 1);
      }
      int w = 255 * k / num_dissolve;

      if (w >= 255) {
        colors[i] = to[i];
      } else {
        colors[i] = rgbEasing(from[i], to[i], w, num_dissolve);
      }
    }
  }

  return progress >= num_pixels + num_dissolve - 1;
}

void slide_dissolve(uint32_t colors[], uint32_t from[], uint32_t to[],
                    uint32_t num_pixels, uint32_t progress,
                    uint32_t num_dissolve = 3) {
  for (size_t i = 0; i < num_pixels; i++) {
    if (progress <= i) {
      colors[i] = from[i];
    } else if (num_dissolve + i <= progress) {
      size_t k = num_pixels - progress + i + num_dissolve - 1;
      colors[i] = to[k];
    } else {
      int k = progress - i;
      //   int w = 255 * k / num_dissolve;

      colors[i] =
          rgbEasing(from[i], to[num_pixels - 1], progress, num_dissolve);
    }
  }
}

/**
 * @brief assign easing color
 *
 * @param colors array of colors
 * @param num_pixels
 * @param left
 * @param right
 */
void gradientColor(uint32_t colors[], size_t num_pixels, uint32_t left,
                   uint32_t right) {
  for (size_t i = 0; i < num_pixels; i++) {
    colors[i] = rgbEasing(left, right, i, num_pixels);
  }
}

void rainbowColor(uint32_t colors[], size_t num_pixels, unsigned char s = 255,
                  unsigned char v = 200) {
  unsigned char hue = 0;

  for (size_t i = 0; i < num_pixels; i++) {
    hue = (255 * i) / num_pixels;
    colors[i] = hsbToHex(hue, s, v);
  }
}
}  // namespace color

#endif
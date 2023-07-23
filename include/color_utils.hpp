
#ifndef COLOR_UTILS_HPP
#define COLOR_UTILS_HPP

#include <Easing.h>
#include <math.h>

namespace easing {
template <typename T>
T remap(T x, T in_min, T in_max, T out_min, T out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
}  // namespace easing

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

  brightness = static_cast<uint8_t>(ma * 255);

  if (std::fabs(delta) < 1e-6) {
    // this is gray
    // hue value is not updated
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

  hue = static_cast<uint16_t>(65535 * f_hue);
}

void hexToHsb(uint32_t color, uint16_t &hue, uint8_t &saturation,
              uint8_t &brightness) {
  uint8_t r, g, b;
  hexToRgb(color, r, g, b);
  rgbToHsb(r, g, b, hue, saturation, brightness);
}

uint32_t rgbEasing(uint32_t from, uint32_t to, float progress) {
  uint8_t rf, gf, bf, wf;
  hexToRgbw(from, rf, gf, bf, wf);
  uint8_t rt, gt, bt, wt;
  hexToRgbw(to, rt, gt, bt, wt);

  EasingFunc<Ease::Linear> easing;
  easing.scale(255.0f);

  uint8_t ri, gi, bi;
  uint8_t w = static_cast<uint8_t>(easing.get(progress));  // weight, 0--255

  ri = easing::remap(w, (uint8_t)0, (uint8_t)255, rf, rt);
  gi = easing::remap(w, (uint8_t)0, (uint8_t)255, gf, gt);
  bi = easing::remap(w, (uint8_t)0, (uint8_t)255, bf, bt);
  uint32_t color = ri << 16 | gi << 8 | bi;
  return color;
}

uint32_t hsbEasing(uint32_t from, uint32_t to, float progress) {
  uint16_t h0, h1;
  uint8_t s0, s1, b0, b1;

  hexToHsb(from, h0, s0, b0);
  hexToHsb(to, h1, s1, b1);

  EasingFunc<Ease::Linear> easing;
  uint16_t w16 =
      static_cast<uint16_t>(0xFFFF * easing.get(progress));       // weight
  uint8_t w8 = static_cast<uint8_t>(255 * easing.get(progress));  // weight

  uint16_t h = easing::remap(w16, (uint16_t)0, (uint16_t)0xFFFF, h0, h1);
  uint8_t s = easing::remap(w8, (uint8_t)0, (uint8_t)0xFF, s0, s1);
  uint8_t b = easing::remap(w8, (uint8_t)0, (uint8_t)0xFF, b0, b1);

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
        colors[i] = hsbEasing(palette[0], palette[1],
                              static_cast<float>(i) / num_pixels);
      }

      break;

    default:
      break;
  }
}

bool dissolveEasing(uint32_t colors[], uint32_t from[], uint32_t to[],
                    uint32_t num_pixels, float progress) {
  // uint32_t pixel_progress = static_cast<uint32_t>(
  //     num_pixels * (static_cast<float>(progress) / duration));
  for (uint32_t i = 0; i < num_pixels; i++) {
    colors[i] = hsbEasing(from[i], to[i], progress);
  }
  return progress >= 1.0f;
}

bool wipeEasing(uint32_t colors[], uint32_t from[], uint32_t to[],
                uint16_t num_pixels, float progress, bool backward = false) {
  if (progress < 0.0f) {
    for (uint16_t i = 0; i < num_pixels; i++) {
      colors[i] = from[i];
    }
  } else if (progress > 1.0f) {
    for (uint16_t i = 0; i < num_pixels; i++) {
      colors[i] = to[i];
    }
  }

  uint32_t pixel_progress = static_cast<uint32_t>(num_pixels * progress);

  for (uint32_t i = 0; i < num_pixels; i++) {
    bool non_wiped;
    if (!backward) {
      non_wiped = pixel_progress - 1 < i;
    } else {
      non_wiped = (num_pixels - pixel_progress - 1) < i;
    }
    if (non_wiped) {
      colors[i] = from[i];
    } else {
      colors[i] = to[i];
    }
  }

  return progress >= 1.0f;
}

// bool slideUpdate(uint32_t colors[], uint32_t from[], uint32_t to[],
//                  uint32_t num_pixels, uint32_t progress, bool forward = true)
//                  {
//   uint32_t head;
//   uint32_t tail;
//   bool b;
//   for (size_t i = 0; i < num_pixels; i++) {
//     head = progress - 1;
//     tail = head - num_pixels + 1;
//     if (forward) {
//       b = head < i;
//     } else {
//       b = i < tail;
//     }

//     if (b) {
//       colors[i] = from[i];
//     } else {
//       if (forward) {
//         colors[i] = to[num_pixels - (head - i) - 1];
//       } else {
//         colors[i] = to[i - tail];
//       }
//     }
//   }

//   return progress >= num_pixels;
// }

// bool wipe_dissolve(uint32_t colors[], uint32_t from[], uint32_t to[],
//                    uint32_t num_pixels, uint32_t progress,
//                    uint32_t num_dissolve = 3, bool forward = true) {
//   bool b;
//   // num_seq = num_pixels + num_dissolve
//   for (size_t i = 0; i < num_pixels; i++) {
//     if (forward) {
//       b = progress < i + 1;
//     } else {
//       b = num_pixels - progress - 1;
//     }

//     if (b) {
//       colors[i] = from[i];
//     } else {
//       int k = 0;
//       if (forward) {
//         k = progress - i;
//       } else {
//         k = i - (num_pixels - progress - 1);
//       }
//       int w = 255 * k / num_dissolve;

//       if (w >= 255) {
//         colors[i] = to[i];
//       } else {
//         colors[i] = rgbEasing(from[i], to[i], w, num_dissolve);
//       }
//     }
//   }

//   return progress >= num_pixels + num_dissolve - 1;
// }

// void slide_dissolve(uint32_t colors[], uint32_t from[], uint32_t to[],
//                     uint32_t num_pixels, uint32_t progress,
//                     uint32_t num_dissolve = 3) {
//   for (size_t i = 0; i < num_pixels; i++) {
//     if (progress <= i) {
//       colors[i] = from[i];
//     } else if (num_dissolve + i <= progress) {
//       size_t k = num_pixels - progress + i + num_dissolve - 1;
//       colors[i] = to[k];
//     } else {
//       int k = progress - i;
//       //   int w = 255 * k / num_dissolve;

//       colors[i] =
//           rgbEasing(from[i], to[num_pixels - 1], progress, num_dissolve);
//     }
//   }
// }

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
    colors[i] = rgbEasing(left, right, static_cast<float>(i) / num_pixels);
  }
}

void rainbowColor(uint32_t colors[], size_t num_pixels, unsigned char s = 255,
                  unsigned char v = 200) {
  unsigned char hue = 0;

  for (size_t i = 0; i < num_pixels; i++) {
    hue = (0xFFFF * i) / num_pixels;
    colors[i] = hsbToHex(hue, s, v);
  }
}
}  // namespace color

#endif
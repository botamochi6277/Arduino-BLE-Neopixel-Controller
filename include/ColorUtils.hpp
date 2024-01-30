
#ifndef COLOR_UTILS_HPP
#define COLOR_UTILS_HPP

// #include <Easing.h>
#include <Arduino.h>
#include <math.h>

#include "Colormap.hpp"
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

uint32_t hsbToHsbhex(uint16_t hue, uint8_t saturation, uint8_t brightness) {
  return ((uint32_t)hue << 16 | (uint16_t)saturation << 8 | brightness);
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

void hsbhexToHsb(uint32_t hsb_color, uint16_t &hue, uint8_t &saturation,
                 uint8_t &brightness) {
  hue = (0xFFFF0000 & hsb_color) >> 16;
  saturation = (0x0000FF00 & hsb_color) >> 8;
  brightness = (0x000000FF & hsb_color);
}

uint32_t hsbhexTohex(int32_t hsb_color) {
  uint16_t h0;
  uint8_t s0, b0;
  hsbhexToHsb(hsb_color, h0, s0, b0);
  return hsbToHex(h0, s0, b0);
}

uint32_t rgbEasing(uint32_t from, uint32_t to, float progress) {
  uint8_t rf, gf, bf, wf;
  hexToRgbw(from, rf, gf, bf, wf);
  uint8_t rt, gt, bt, wt;
  hexToRgbw(to, rt, gt, bt, wt);

  uint8_t ri, gi, bi;
  uint8_t w = static_cast<uint8_t>(easing::remap(
      progress, 0.0f, 1.0f, 0.0f, 255.0f, true));  // weight, 0--255

  ri = easing::remap(w, (uint8_t)0, (uint8_t)255, rf, rt, true);
  gi = easing::remap(w, (uint8_t)0, (uint8_t)255, gf, gt, true);
  bi = easing::remap(w, (uint8_t)0, (uint8_t)255, bf, bt, true);
  uint32_t color = ri << 16 | gi << 8 | bi;
  return color;
}

uint32_t hsbhexEasing(uint32_t hsb_from, uint32_t hsb_to, float progress) {
  if (progress < 0.0f) {
    return hsb_from;
  }
  if (0.99f < progress) {
    return hsb_to;
  }

  uint16_t h0, h1;
  uint8_t s0, s1, b0, b1;

  hsbhexToHsb(hsb_from, h0, s0, b0);
  hsbhexToHsb(hsb_to, h1, s1, b1);

  uint16_t w16 = static_cast<uint16_t>(easing::remap(
      progress, 0.0f, 1.0f, 0.0f, static_cast<float>(0xFFFF), true));  // weight
  uint8_t w8 = static_cast<uint8_t>(
      easing::remap(progress, 0.0f, 1.0f, 0.0f, 255.0f, true));  // weight

  uint16_t h = easing::remap(w16, (uint16_t)0, (uint16_t)0xFFFF, h0, h1, true);
  uint8_t s = easing::remap(w8, (uint8_t)0, (uint8_t)0xFF, s0, s1, true);
  uint8_t b = easing::remap(w8, (uint8_t)0, (uint8_t)0xFF, b0, b1, true);

  return hsbToHsbhex(h, s, b);
}
}  // namespace color

#endif
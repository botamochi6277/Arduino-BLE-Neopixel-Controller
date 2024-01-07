
#ifndef COLOR_UTILS_HPP
#define COLOR_UTILS_HPP

// #include <Easing.h>
#include <math.h>

#include "easing.hpp"
namespace color {

void hueCycle(float &r, float &g, float &b, float time, float period,
              float initial_phase);

class PixelUnit {
 private:
  float red_;  // 0.0 -- 1.0
  float green_;
  float blue_;
  float position_;

 public:
  PixelUnit();
  PixelUnit(float red, float green, float blue, float position);
  PixelUnit(unsigned int hexcolor, float position);
  void setColor(float red, float green, float blue);
  void setColor(unsigned int hexcolor);
  void setHueCycle(float time, float period, float initial_phase);
  void setPosition(float position);
  void blendFromAnchors(PixelUnit &anchor1, PixelUnit &anchor2);
  float red();
  float green();
  float blue();
  float position();
};

PixelUnit::PixelUnit() {
  this->setColor(0.0f, 0.0f, 0.0f);
  this->setPosition(0.0f);
}
PixelUnit::PixelUnit(float red, float green, float blue, float position) {
  this->setColor(red, green, blue);
  this->setPosition(position);
}
PixelUnit::PixelUnit(unsigned int hexcolor, float position) {
  this->setColor(hexcolor);
  this->setPosition(position);
}
void PixelUnit::setColor(float red, float green, float blue) {
  this->red_ = red;
  this->green_ = green;
  this->blue_ = blue;
}

void PixelUnit::setColor(unsigned int hexcolor) {
  this->red_ = ((0x00FF0000 & hexcolor) >> 16) / 255.0f;
  this->green_ = ((0x0000FF00 & hexcolor) >> 8) / 255.0f;
  this->blue_ = ((0x000000FF & hexcolor)) / 255.0f;
}

void PixelUnit::setHueCycle(float time, float period, float initial_phase) {
  hueCycle(this->red_, this->green_, this->blue_, time, period, initial_phase);
}

float PixelUnit::red() { return this->red_; }
float PixelUnit::green() { return this->green_; }
float PixelUnit::blue() { return this->blue_; }
float PixelUnit::position() { return this->position_; }
void PixelUnit::setPosition(float position) { this->position_ = position; }

void PixelUnit::blendFromAnchors(PixelUnit &anchor1, PixelUnit &anchor2) {
  float distance1 = abs(anchor1.position() - this->position_);
  float distance2 = abs(anchor2.position() - this->position_);
  float score1 = 1.0f / (distance1 + 1e-9f);
  float score2 = 1.0f / (distance2 + 1e-9f);
  float weight1 = score1 / (score1 + score2);
  float weight2 = score2 / (score1 + score2);

  // blend with weight
  // rgb blending
  this->red_ = weight1 * anchor1.red() + weight2 * anchor2.red();
  this->green_ = weight1 * anchor1.green() + weight2 * anchor2.green();
  this->blue_ = weight1 * anchor1.blue() + weight2 * anchor2.blue();
}

void hueCycle(float &r, float &g, float &b, float time, float period,
              float initial_phase) {
  // (1,0,0),(1,1,0),(0,0,1),(0,1,1),(0,0,1),(1,0,1)
  // https://ja.wikipedia.org/wiki/カラーグラデーション#/media/ファイル:Gnuplot_HSV_gradient.png
  float phase = 2.0f * M_PI * (time / period) + initial_phase - M_PI / 6.0f;
  float a = 0.5773502691896258;  // 1/np.sqrt(3);
  r = constrain(sinf(phase + 2.0f * M_PI / 3.0f) + 0.5f, 0.0f, 1.0f);
  g = constrain(sinf(phase) + 0.5f, 0.0f, 1.0f);
  b = constrain(sinf(phase - 2.0f * M_PI / 3.0f) + 0.5f, 0.0f, 1.0f);
}

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
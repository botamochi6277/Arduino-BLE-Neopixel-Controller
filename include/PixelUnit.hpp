#ifndef PIXEL_UNIT_HPP
#define PIXEL_UNIT_HPP

#include <math.h>

#include "ColorUtils.hpp"
#include "Colormap.hpp"

namespace color {

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
  void setCmapColor(float intensity, colormap::ColormapId cmap,
                    float initial_phase = 0.0f);
  void setPosition(float position);
  void blendFromAnchors(PixelUnit &anchor1, PixelUnit &anchor2);
  float red();
  float green();
  float blue();
  float position();
  unsigned int hex();
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

void PixelUnit::setCmapColor(float intensity, colormap::ColormapId cmap,
                             float initial_phase) {
  colormap::assignColor(this->red_, this->green_, this->blue_, intensity, cmap,
                        initial_phase);
}

float PixelUnit::red() { return this->red_; }
float PixelUnit::green() { return this->green_; }
float PixelUnit::blue() { return this->blue_; }
unsigned int PixelUnit::hex() {
  return (static_cast<uint8_t>(255.0f * this->red_) << 16) |
         (static_cast<uint8_t>(255.0f * this->green_) << 8) |
         (static_cast<uint8_t>(255.0f * this->blue_));
}
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

}  // namespace color
#endif
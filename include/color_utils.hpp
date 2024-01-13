
#ifndef COLOR_UTILS_HPP
#define COLOR_UTILS_HPP

// #include <Easing.h>
#include <math.h>

#include "easing.hpp"
namespace color {

enum class CyclicColormap : unsigned char { Twilight, TwilightShifted, Hsv };
enum class SequentialColormap : unsigned char { Cool, Wistia, Hot };
enum class DivergingColormap : unsigned char { Spectral, CoolWarm };
float cyclicValue(float time, float period, float initial_phase, float a0,
                  float a[], float b[], float f[], size_t coffs_length);

float polyValue(float x, float coffs[], size_t coffs_length);
void setCyclicColor(float &r, float &g, float &b, float time, float period,
                    float initial_phase, CyclicColormap cmap);

void setHsvCyclicColor(float &r, float &g, float &b, float time, float period,
                       float initial_phase);

void heatColor(float &r, float &g, float &b, float temperature);

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
  void setHeatColor(float temperature);
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

void PixelUnit::setHueCycle(float time, float period, float initial_phase) {
  setHsvCyclicColor(this->red_, this->green_, this->blue_, time, period,
                    initial_phase);
}
void PixelUnit::setHeatColor(float temperature) {
  heatColor(this->red_, this->green_, this->blue_, temperature);
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

float cyclicValue(float time, float period, float initial_phase, float a0,
                  float a[], float b[], float f[], size_t coffs_length) {
  float x = a0;  // DC component
  for (size_t i = 0; i < coffs_length; i++) {
    // e^((a + j*b)*t)
    x += a[i] * cos(2.0f * M_PI * f[i] * (time / period) + initial_phase) -
         b[i] * sin(2.0f * M_PI * f[i] * (time / period) + initial_phase);
  }
  return x;
}

float polyValue(float x, float coffs[], size_t coffs_length) {
  float y = 0.0f;
  for (size_t i = 0; i < coffs_length; i++) {
    if (i == 0) {
      y += coffs[i];
    } else {
      y += powf(x, i);
    }
  }
  return y;
}

void setCyclicColor(float &r, float &g, float &b, float time, float period,
                    float initial_phase, CyclicColormap cmap) {
  // a+bj, f
  // hsv param
  static float hsv_r_a0 = 4.8585e-01f;
  static float hsv_r_a[] = {6.0380e-01f, -1.2575e-01f};
  static float hsv_r_b[] = {-1.0742e-03, 9.5391e-04};
  static float hsv_r_f[] = {9.8438e-01, 2.9531e+00};

  static float hsv_g_a0 = 5.1379e-01f;
  static float hsv_g_a[] = {-2.9102e-01f, -1.3128e-01f};
  static float hsv_g_b[] = {-5.3190e-01, -9.5479e-03};
  static float hsv_g_f[] = {9.8438e-01, 2.9531e+00};

  static float hsv_b_a0 = 5.1249e-01;
  static float hsv_b_a[] = {-2.9224e-01, -1.3310e-01};
  static float hsv_b_b[] = {5.3225e-01, 1.0011e-02};
  static float hsv_b_f[] = {9.8438e-01, 2.9531e+00};

  if (cmap == CyclicColormap::Hsv) {
    r = cyclicValue(time, period, initial_phase, hsv_r_a0, hsv_r_a, hsv_r_b,
                    hsv_r_f, 2U);
    g = cyclicValue(time, period, initial_phase, hsv_g_a0, hsv_g_a, hsv_g_b,
                    hsv_g_f, 2U);
    b = cyclicValue(time, period, initial_phase, hsv_b_a0, hsv_b_a, hsv_b_b,
                    hsv_b_f, 2U);
  }

  // twilight
  static float tw_r_a0 = 5.3852e-01f;
  static float tw_r_a[] = {2.5764e-01, 5.9289e-02};
  static float tw_r_b[] = {1.4902e-01, -1.3312e-02};
  static float tw_r_f[] = {9.8438e-01, 2.9531e+00};

  static float tw_g_a0 = 4.1168e-01f;
  static float tw_g_a[] = {3.8150e-01, 3.0205e-02};
  static float tw_g_b[] = {-3.3706e-02, -3.1774e-03};
  static float tw_g_f[] = {9.8438e-01, 1.9688e+00};

  static float tw_b_a0 = 5.3664e-01;
  static float tw_b_a[] = {2.4245e-01, 6.7400e-02};
  static float tw_b_b[] = {-1.7885e-01, 2.8135e-02};
  static float tw_b_f[] = {9.8438e-01, 2.9531e+00};
  if (cmap == CyclicColormap::Twilight) {
    r = cyclicValue(time, period, initial_phase, tw_r_a0, tw_r_a, tw_r_b,
                    tw_r_f, 2U);
    g = cyclicValue(time, period, initial_phase, tw_g_a0, tw_g_a, tw_g_b,
                    tw_g_f, 2U);
    b = cyclicValue(time, period, initial_phase, tw_b_a0, tw_b_a, tw_b_b,
                    tw_b_f, 2U);
  }

  // twilight_shifted
  static float tws_r_a0 = 5.4938e-01f;
  static float tws_r_a[] = {-2.7070e-01, -5.1723e-02};
  static float tws_r_b[] = {1.2390e-01, -3.2384e-02};
  static float tws_r_f[] = {9.8438e-01, 2.9531e+00};

  static float tws_g_a0 = 4.2379e-01;
  static float tws_g_a[] = {-3.7971e-01, 2.2300e-02};
  static float tws_g_b[] = {-7.1732e-02, 5.3167e-03};
  static float tws_g_f[] = {9.8438e-01, 1.9688e+00};

  static float tws_b_a0 = 5.4709e-01;
  static float tws_b_a[] = {-2.2441e-01, -7.1713e-02};
  static float tws_b_b[] = {-2.0186e-01, 7.4686e-03};
  static float tws_b_f[] = {9.8438e-01, 2.9531e+00};

  if (cmap == CyclicColormap::TwilightShifted) {
    r = cyclicValue(time, period, initial_phase, tw_r_a0, tw_r_a, tw_r_b,
                    tw_r_f, 2U);
    g = cyclicValue(time, period, initial_phase, tw_g_a0, tw_g_a, tw_g_b,
                    tw_g_f, 2U);
    b = cyclicValue(time, period, initial_phase, tw_b_a0, tw_b_a, tw_b_b,
                    tw_b_f, 2U);
  }
  r = constrain(r, 0.0f, 1.0f);
  g = constrain(g, 0.0f, 1.0f);
  b = constrain(b, 0.0f, 1.0f);
  return;
}

void setHsvCyclicColor(float &r, float &g, float &b, float time, float period,
                       float initial_phase) {
  // (1,0,0),(1,1,0),(0,0,1),(0,1,1),(0,0,1),(1,0,1)
  // https://ja.wikipedia.org/wiki/カラーグラデーション#/media/ファイル:Gnuplot_HSV_gradient.png
  float phase = 2.0f * M_PI * (time / period) + initial_phase - M_PI / 6.0f;
  r = constrain(sinf(phase + 2.0f * M_PI / 3.0f) + 0.5f, 0.0f, 1.0f);
  g = constrain(sinf(phase) + 0.5f, 0.0f, 1.0f);
  b = constrain(sinf(phase - 2.0f * M_PI / 3.0f) + 0.5f, 0.0f, 1.0f);
}

void setSequentialColor(float &r, float &g, float &b, float intensity,
                        SequentialColormap cmap) {
  static float cool_r[] = {1.1765e-02, 9.7647e-01};
  static float cool_g[] = {9.8824e-01, -9.7647e-01};
  static float cool_b[] = {1.0000e+00, -7.3651e-17};
  if (cmap == SequentialColormap::Cool) {
    r = polyValue(intensity, cool_r, 2U);
    g = polyValue(intensity, cool_g, 2U);
    b = polyValue(intensity, cool_b, 2U);
  }
  static float hot_r[] = {7.2484e-02, 3.0686e+00, -9.9480e-01, -4.2600e+00,
                          3.1138e+00};
  static float hot_g[] = {1.9271e-07, -5.2787e-01, 6.9576e-01, 6.2264e+00,
                          -5.3943e+00};
  static float hot_b[] = {-5.5203e-08, 8.4880e-02, 2.9603e-01, -2.8484e+00,
                          3.4211e+00};
  if (cmap == SequentialColormap::Hot) {
    r = polyValue(intensity, hot_r, 5U);
    g = polyValue(intensity, hot_g, 5U);
    b = polyValue(intensity, hot_b, 5U);
  }

  static float wistia_r[] = {8.9910e-01, 5.3242e-01, -8.2493e-01, 3.8220e-01};
  static float wistia_g[] = {9.9576e-01, -3.3824e-01, -3.9361e-01, 2.4023e-01};
  static float wistia_b[] = {4.6072e-01, -1.9199e+00, 2.5792e+00, -1.1200e+00};
  //   red coff: 8.9910e-01,5.3242e-01,-8.2493e-01,3.8220e-01
  // green coff: 9.9576e-01,-3.3824e-01,-3.9361e-01,2.4023e-01
  // blue coff: 4.6072e-01,-1.9199e+00,2.5792e+00,-1.1200e+00
  if (cmap == SequentialColormap::Wistia) {
    r = polyValue(intensity, wistia_r, 4U);
    g = polyValue(intensity, wistia_g, 4U);
    b = polyValue(intensity, wistia_b, 4U);
  }
  r = constrain(r, 0.0f, 1.0f);
  g = constrain(g, 0.0f, 1.0f);
  b = constrain(b, 0.0f, 1.0f);
  return;
}

void setDivergingColor(float &r, float &g, float &b, float intensity,
                       DivergingColormap cmap) {
  static float spectral_r[] = {6.4498e-01, 1.1186e+00, 4.1186e+00, -1.4378e+01,
                               8.8443e+00};
  static float spectral_g[] = {3.2065e-02, 1.7566e+00, 4.2265e+00, -1.0415e+01,
                               4.7360e+00};
  static float spectral_b[] = {2.6482e-01, -6.5753e-01, 6.5861e+00, -9.8745e+00,
                               4.3289e+00};
  if (cmap == DivergingColormap::Spectral) {
    r = polyValue(intensity, spectral_r, 5U);
    g = polyValue(intensity, spectral_g, 5U);
    b = polyValue(intensity, spectral_b, 5U);
  }

  static float cool_warm_r[] = {2.4352e-01, 1.9864e+00, -1.5066e+00};
  static float cool_warm_g[] = {3.1919e-01, 2.2997e+00, -2.5500e+00};
  static float cool_warm_b[] = {7.7167e-01, 9.1354e-01, -1.5222e+00};
  if (cmap == DivergingColormap::CoolWarm) {
    r = polyValue(intensity, cool_warm_r, 3U);
    g = polyValue(intensity, cool_warm_g, 3U);
    b = polyValue(intensity, cool_warm_b, 3U);
  }

  r = constrain(r, 0.0f, 1.0f);
  g = constrain(g, 0.0f, 1.0f);
  b = constrain(b, 0.0f, 1.0f);
  return;
}

void heatColor(float &r, float &g, float &b, float temperature) {
  // https://ja.wikipedia.org/wiki/カラーグラデーション#/media/ファイル:P_hot.gif
  r = easing::hardSigmoid(temperature, 0.333f, 1.0f);
  g = easing::hardSigmoid(temperature - 0.333f, 0.333f, 1.0f);
  b = easing::hardSigmoid(temperature - 0.666f, 0.333f, 1.0f);
}

void polyRgb(float &r, float &g, float &b, float t, uint8_t order,
             float r_coffs[], float g_coffs[], float b_coffs[]) {
  r = 0.0f;
  g = 0.0f;
  b = 0.0f;
  for (size_t i = 0; i < order; i++) {
    r += r_coffs[i] * powf(t, i);
    g += g_coffs[i] * powf(t, i);
    b += b_coffs[i] * powf(t, i);
  }
}

void viridisColor(float &r, float &g, float &b, float t) {
  // order = 5
  static float r_coffs[] = {0.27130491f, 0.81659759f, -6.19363115f, 9.76791191f,
                            -3.69728915f};
  static float g_coffs[] = {0.019942f, 1.51977397f, -1.60202556f, 2.03930642f,
                            -1.07467385f};
  static float b_coffs[] = {0.34726911f, 1.05768234f, -0.52648131f,
                            -2.21724279f, 1.46271341f};
  polyRgb(r, g, b, t, 5, r_coffs, g_coffs, b_coffs);
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
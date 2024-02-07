#ifndef BOTALAB_EASING_HPP
#define BOTALAB_EASING_HPP
#include <cmath>

// https://easings.net
namespace easing {

template <typename T>
uint8_t argmax3(T v0, T v1, T v2) {
  uint8_t i = 0;
  if (v0 < v1) {
    i = 1;
  }
  if ((v0 < v2) && (v1 < v2)) {
    i = 2;
  }
  return i;
}

template <typename T>
T remap(T x, T in_min, T in_max, T out_min, T out_max, bool clip = false) {
  if (clip) {
    if (x < in_min) {
      return out_min;
    }
    if (in_max < x) {
      return out_max;
    }
  }

  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

float hardSigmoid(float x) {
  if (x < -0.5f) {
    return 0.0f;
  } else if (x < 0.5f) {
    return x + 0.5f;
  } else {
    return 1.0f;
  }
}

float hardSigmoidInOut(float x, float duration = 1.0f, float scale = 1.0f) {
  return scale * hardSigmoid((x - 0.5f) / (duration + 1e-9f));
}

float trianglePulse(float x, float duration = 1.0f, float scale = 1.0f) {
  return scale * (hardSigmoid((2.0f * (x + 0.5f)) / (duration + 1.0e-9f)) -
                  hardSigmoid((2.0f * (x - 0.5f)) / (duration + 1.0e-9f)));
}

float quadInOut(float x, float duration = 1.0f, float scale = 1.0f) {
  static float xx = x / (duration + 1e-9f);
  if (x < 0.0f) {
    return 0.0f;
  } else if (xx < 0.5f) {
    return scale * 2.0f * xx * xx;
  } else if (xx < 1.0f) {
    return scale * (1.0f - std::pow(-2.0f * xx + 2.0f, 2.0f) / 2.0f);
  } else {
    return scale;
  }
}

float quadPulse(float x, float duration = 1.0f, float scale = 1.0f) {
  return scale * (quadInOut(x, duration / 2.0f, 1.0f) -
                  quadInOut(x - duration / 2.0f, duration / 2.0f, 1.0f));
}

}  // namespace easing

#endif
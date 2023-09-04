#ifndef BOTALAB_EASING_HPP
#define BOTALAB_EASING_HPP
#include <cmath>

// https://easings.net
namespace easing {
template <typename T>
T remap(T x, T in_min, T in_max, T out_min, T out_max, bool chip = false) {
  if (chip) {
    if (x < in_min) {
      return out_min;
    }
    if (in_max < x) {
      return out_max;
    }
  }

  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

float hardSigmoid(float x, float duration = 1.0, float scale = 1.0) {
  if (x < 0.0) {
    return 0.0;
  } else if (0.0 <= x && x < duration) {
    return (scale / duration) * x;
  } else {
    return 1.0;
  }
}

float trianglePulse(float x, float duration = 1.0, float scale = 1.0) {
  return scale * (hardSigmoid(x, duration / 2.0, 1.0) -
                  hardSigmoid(x - duration / 2.0, duration / 2.0, 1.0));
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
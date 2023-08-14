#ifndef BOTALAB_EASING_HPP
#define BOTALAB_EASING_HPP
#include <math.h>

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
}  // namespace easing

#endif
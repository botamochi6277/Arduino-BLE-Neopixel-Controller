#ifndef EASING_HPP
#define EASING_HPP
// #include <Arduino.h>
#include <math.h>

// https://easings.net
namespace easing {

const short LINEAR = 0;
const short SINE = 1;

template <typename T>
T remap(T x, T in_min, T in_max, T out_min, T out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
/**
 * @brief linear easing
 *
 * @param progress
 * @param durations
 * @return T 0--a
 */
template <typename T>
T linear(T progress, T duration = 255, T a = 255) {
  if (progress < 0) {
    return 0;
  }
  if (progress >= duration) {
    return a;
  }
  T v = remap(progress, (T)0, duration, (T)0, a);
  return v;
}
template <typename T>
T inOutSine(T progress, T duration = 255, T a = 255) {
  if (progress < 0) {
    return 0;
  }
  if (progress >= duration) {
    return a;
  }
  float v = sin((float)progress / duration * PI - 0.5f * PI);  // [-1,1]
  return remap(v, -1.0f, 1.0f, 0.0f, static_cast<float>(a));
}

int inSine(int progress, int duration = 255) {
  if (progress < 0) {
    return 0;
  }
  if (progress >= duration) {
    return 255;
  }
  float v = sin(progress / duration * 0.5f * PI - 0.5f * PI);  // [-1,0]

  return remap(v, -1.0f, 0.0f, 0.0f, 255.0f);
}

int outSine(int progress, int duration = 255) {
  if (progress < 0) {
    return 0;
  }
  if (progress >= duration) {
    return 255;
  }
  float v = sin(progress / duration * 0.5f * PI);  //[0,1]
  return remap(v, 0.0f, 1.0f, 0.0f, 255.0f);
}

}  // namespace easing

#endif
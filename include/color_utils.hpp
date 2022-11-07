
#ifndef COLOR_UTILS_HPP
#define COLOR_UTILS_HPP
#include <Arduino.h>

#include "easing.hpp"

void colorToHex(unsigned int c, unsigned char &r, unsigned char &g,
                unsigned char &b, unsigned char &w) {
  // 0x FF 00 FF 00
  w = (0xFF000000 & c) >> 24;
  r = (0x00FF0000 & c) >> 16;
  g = (0x0000FF00 & c) >> 8;
  b = (0x000000FF & c);
}

unsigned int colorEasing(unsigned int from, unsigned int to, int frame,
                         int duration, short ease = easing::LINEAR) {
  unsigned char rf, gf, bf, wf;
  colorToHex(from, rf, gf, bf, wf);
  unsigned char rt, gt, bt, wt;
  colorToHex(to, rt, gt, bt, wt);

  unsigned int color;
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

bool wipeUpdate(unsigned int colors[], unsigned int from[], unsigned int to[],
                unsigned int num_pixels, unsigned int progress,
                bool forward = true) {
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

bool dissolveUpdate(unsigned int colors[], unsigned int from[],
                    unsigned int to[], unsigned int num_pixels,
                    unsigned int progress, unsigned int duration) {
  // float w = progress / num_pixels;
  for (size_t i = 0; i < num_pixels; i++) {
    // unsigned unsigned int r = w * (to[i] - from[i]) + from[i];
    colors[i] = colorEasing(from[i], to[i], progress, duration);
  }
  return progress >= duration;
}

bool slideUpdate(unsigned int colors[], unsigned int from[], unsigned int to[],
                 unsigned int num_pixels, unsigned int progress,
                 bool forward = true) {
  unsigned int head;
  unsigned int tail;
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

bool wipe_dissolve(unsigned int colors[], unsigned int from[],
                   unsigned int to[], unsigned int num_pixels,
                   unsigned int progress, unsigned int num_dissolve = 3,
                   bool forward = true) {
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
        colors[i] = colorEasing(from[i], to[i], w, num_dissolve);
      }
    }
  }

  return progress >= num_pixels + num_dissolve - 1;
}

void slide_dissolve(unsigned int colors[], unsigned int from[],
                    unsigned int to[], unsigned int num_pixels,
                    unsigned int progress, unsigned int num_dissolve = 3) {
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
          colorEasing(from[i], to[num_pixels - 1], progress, num_dissolve);
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
void gradient_color(unsigned int colors[], size_t num_pixels, unsigned int left,
                    unsigned int right) {
  for (size_t i = 0; i < num_pixels; i++) {
    colors[i] = colorEasing(left, right, i, num_pixels);
  }
}

#endif
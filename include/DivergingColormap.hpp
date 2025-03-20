#ifndef BOTALAB_DIVERGING_COLORMAP_HPP
#define BOTALAB_DIVERGING_COLORMAP_HPP
#include <Arduino.h>
#include <math.h>
#define POLY_ORDER 5
namespace colormap::diverging {
class PolyParam {
   private:
    float r_coffs_[POLY_ORDER];
    float g_coffs_[POLY_ORDER];
    float b_coffs_[POLY_ORDER];

   public:
    PolyParam(/* args */);
    PolyParam(float r_coffs[], float g_coffs[], float b_coffs[])
        : r_coffs_(
              {r_coffs[0], r_coffs[1], r_coffs[2], r_coffs[3], r_coffs[4]}),
          g_coffs_(
              {g_coffs[0], g_coffs[1], g_coffs[2], g_coffs[3], g_coffs[4]}),
          b_coffs_(
              {b_coffs[0], b_coffs[1], b_coffs[2], b_coffs[3], b_coffs[4]}) {}

    //   for inline instance
    PolyParam(float r0, float r1, float r2, float r3, float r4,  // r
              float g0, float g1, float g2, float g3, float g4,  // g
              float b0, float b1, float b2, float b3, float b4)  // b
        : r_coffs_({r0, r1, r2, r3, r4}),
          g_coffs_({g0, g1, g2, g3, g4}),
          b_coffs_({b0, b1, b2, b3, b4}) {}

    float* r_coffs() { return r_coffs_; }
    float* g_coffs() { return g_coffs_; }
    float* b_coffs() { return b_coffs_; }
};

// presets

PolyParam ARGON = PolyParam(0.451f, 0.748f, 2.904f, -6.265f, 3.126f,   // r
                            0.012f, 0.987f, 8.295f, -17.964f, 8.979f,  // g
                            0.753f, -0.028f, 4.416f, -9.597f, 4.805f   // b
);

PolyParam JSHINE = PolyParam(0.071f, 1.090f, 2.513f, -5.394f, 2.685f,    // r
                             0.761f, -0.521f, -0.928f, 1.984f, -0.986f,  // g
                             0.914f, -0.311f, 2.919f, -6.362f, 3.189f    // b
);

PolyParam RELAY = PolyParam(0.227f, 0.952f, 2.296f, -4.929f, 2.454f,  // r
                            0.110f, 0.593f, 0.314f, -0.645f, 0.314f,  // g
                            0.941f, -0.344f, 1.278f, -2.799f, 1.406f  // b
);

PolyParam COOLWARM = PolyParam(0.000f, 1.400f, 4.976f, -10.725f, 5.349f,   // r
                               0.000f, 0.826f, 9.870f, -21.396f, 10.700f,  // g
                               1.000f, -0.574f, 4.895f, -10.672f, 5.350f   // b
);

PolyParam BACK_TO_EARTH =
    PolyParam(0.000f, 1.155f, 7.068f, -15.286f, 7.636f,  // r
              0.788f, 0.294f, 1.073f, -2.313f, 1.154f,   // g
              1.000f, -0.220f, 1.881f, -4.101f, 2.056f   // b
    );

PolyParam IRIS = PolyParam(0.667f, 0.467f, 1.659f, -3.575f, 1.783f,   // r
                           0.000f, 1.400f, 4.976f, -10.725f, 5.349f,  // g
                           1.000f, -0.574f, 4.895f, -10.672f, 5.350f  // b
);
// end of presets

float polyValue(float x, const float coffs[], unsigned char coffs_length) {
    float y = 0.0f;
    for (unsigned char i = 0; i < coffs_length; i++) {
        if (i == 0) {
            y += coffs[i];
        } else {
            y += coffs[i] * powf(x, i);
        }
    }
    return y;
}

void assignPolyColor(float& r, float& g, float& b, float intensity,
                     PolyParam cmap) {
    r = polyValue(intensity, cmap.r_coffs(), POLY_ORDER);
    g = polyValue(intensity, cmap.g_coffs(), POLY_ORDER);
    b = polyValue(intensity, cmap.b_coffs(), POLY_ORDER);

    r = constrain(r, 0.0f, 1.0f);
    g = constrain(g, 0.0f, 1.0f);
    b = constrain(b, 0.0f, 1.0f);
}

}  // namespace colormap::diverging
#endif
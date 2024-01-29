#ifndef BOTALAB_COLORMAP_HPP
#define BOTALAB_COLORMAP_HPP
#include <math.h>
namespace colormap {

float polyValue(float x, const float coffs[], size_t coffs_length) {
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

float cyclicValue(float intensity, float initial_phase, float a0,
                  const float a[], const float b[], const float f[],
                  size_t coffs_length) {
  // intensity: e.g. time/wave_period
  float x = a0;  // DC component
  for (size_t i = 0; i < coffs_length; i++) {
    // e^((a + j*b)*t)
    x += a[i] * cos((2.0f * M_PI * f[i] * intensity) + initial_phase) -
         b[i] * sin((2.0f * M_PI * f[i] * intensity) + initial_phase);
  }
  return x;
}

enum class CyclicColormap : unsigned char { Hsv, Twilight, TwilightShifted };

class HsvHueColormap {
 public:
  //  coefficients
  inline const static float r_a0 = 4.8585e-01f;
  inline const static float r_a[] = {6.0380e-01f, -1.2575e-01f};
  inline const static float r_b[] = {-1.0742e-03, 9.5391e-04};
  inline const static float r_f[] = {9.8438e-01, 2.9531e+00};

  inline const static float g_a0 = 5.1379e-01f;
  inline const static float g_a[] = {-2.9102e-01f, -1.3128e-01f};
  inline const static float g_b[] = {-5.3190e-01, -9.5479e-03};
  inline const static float g_f[] = {9.8438e-01, 2.9531e+00};

  inline const static float b_a0 = 5.1249e-01;
  inline const static float b_a[] = {-2.9224e-01, -1.3310e-01};
  inline const static float b_b[] = {5.3225e-01, 1.0011e-02};
  inline const static float b_f[] = {9.8438e-01, 2.9531e+00};

  // methods
  void assignColor(float &r, float &g, float &b, float intensity,
                   float initial_phase = 0.0f);
};
void HsvHueColormap::assignColor(float &r, float &g, float &b, float intensity,
                                 float initial_phase = 0.0f) {
  r = cyclicValue(intensity, initial_phase, this->r_a0, this->r_a, this->r_b,
                  this->r_f, 2U);
  g = cyclicValue(intensity, initial_phase, this->g_a0, this->g_a, this->g_b,
                  this->g_f, 2U);
  b = cyclicValue(intensity, initial_phase, this->b_a0, this->b_a, this->b_b,
                  this->b_f, 2U);
}
// float BaseColormap::coff_red[3] = {1, 2, 3};
class TwilightColormap {
 public:
  inline const static float r_a0 = 5.3852e-01f;
  inline const static float r_a[] = {2.5764e-01, 5.9289e-02};
  inline const static float r_b[] = {1.4902e-01, -1.3312e-02};
  inline const static float r_f[] = {9.8438e-01, 2.9531e+00};

  inline const static float g_a0 = 4.1168e-01f;
  inline const static float g_a[] = {3.8150e-01, 3.0205e-02};
  inline const static float g_b[] = {-3.3706e-02, -3.1774e-03};
  inline const static float g_f[] = {9.8438e-01, 1.9688e+00};

  inline const static float b_a0 = 5.3664e-01;
  inline const static float b_a[] = {2.4245e-01, 6.7400e-02};
  inline const static float b_b[] = {-1.7885e-01, 2.8135e-02};
  inline const static float b_f[] = {9.8438e-01, 2.9531e+00};
  // methods
  void assignColor(float &r, float &g, float &b, float intensity,
                   float initial_phase = 0.0f);
};
void TwilightColormap::assignColor(float &r, float &g, float &b,
                                   float intensity,
                                   float initial_phase = 0.0f) {
  r = cyclicValue(intensity, initial_phase, this->r_a0, this->r_a, this->r_b,
                  this->r_f, 2U);
  g = cyclicValue(intensity, initial_phase, this->g_a0, this->g_a, this->g_b,
                  this->g_f, 2U);
  b = cyclicValue(intensity, initial_phase, this->b_a0, this->b_a, this->b_b,
                  this->b_f, 2U);
}

class TwilightShiftedColormap {
 public:
  inline const static float r_a0 = 5.4938e-01f;
  inline const static float r_a[] = {-2.7070e-01, -5.1723e-02};
  inline const static float r_b[] = {1.2390e-01, -3.2384e-02};
  inline const static float r_f[] = {9.8438e-01, 2.9531e+00};

  inline const static float g_a0 = 4.2379e-01;
  inline const static float g_a[] = {-3.7971e-01, 2.2300e-02};
  inline const static float g_b[] = {-7.1732e-02, 5.3167e-03};
  inline const static float g_f[] = {9.8438e-01, 1.9688e+00};

  inline const static float b_a0 = 5.4709e-01;
  inline const static float b_a[] = {-2.2441e-01, -7.1713e-02};
  inline const static float b_b[] = {-2.0186e-01, 7.4686e-03};
  inline const static float b_f[] = {9.8438e-01, 2.9531e+00};
  // methods
  void assignColor(float &r, float &g, float &b, float intensity,
                   float initial_phase = 0.0f);
};
void TwilightShiftedColormap::assignColor(float &r, float &g, float &b,
                                          float intensity,
                                          float initial_phase = 0.0f) {
  r = cyclicValue(intensity, initial_phase, this->r_a0, this->r_a, this->r_b,
                  this->r_f, 2U);
  g = cyclicValue(intensity, initial_phase, this->g_a0, this->g_a, this->g_b,
                  this->g_f, 2U);
  b = cyclicValue(intensity, initial_phase, this->b_a0, this->b_a, this->b_b,
                  this->b_f, 2U);
}

enum class SequentialColormap : unsigned char { Cool, Hot, Wistia };
class CoolColormap {
 public:
  inline const static float coff_r[] = {1.1765e-02, 9.7647e-01};
  inline const static float coff_g[] = {9.8824e-01, -9.7647e-01};
  inline const static float coff_b[] = {1.0000e+00, -7.3651e-17};
  // methods
  void assignColor(float &r, float &g, float &b, float intensity);
};

void CoolColormap::assignColor(float &r, float &g, float &b, float intensity) {
  r = polyValue(intensity, this->coff_r, 2U);
  g = polyValue(intensity, this->coff_g, 2U);
  b = polyValue(intensity, this->coff_b, 2U);
}

class HotColormap {
 public:
  inline const static float coff_r[] = {7.2484e-02, 3.0686e+00, -9.9480e-01,
                                        -4.2600e+00, 3.1138e+00};
  inline const static float coff_g[] = {1.9271e-07, -5.2787e-01, 6.9576e-01,
                                        6.2264e+00, -5.3943e+00};
  inline const static float coff_b[] = {-5.5203e-08, 8.4880e-02, 2.9603e-01,
                                        -2.8484e+00, 3.4211e+00};
  // methods
  void assignColor(float &r, float &g, float &b, float intensity);
};

void HotColormap::assignColor(float &r, float &g, float &b, float intensity) {
  r = polyValue(intensity, this->coff_r, 5U);
  g = polyValue(intensity, this->coff_g, 5U);
  b = polyValue(intensity, this->coff_b, 5U);
}

class WistiaColormap {
 public:
  inline const static float coff_r[] = {8.9910e-01, 5.3242e-01, -8.2493e-01,
                                        3.8220e-01};
  inline const static float coff_g[] = {9.9576e-01, -3.3824e-01, -3.9361e-01,
                                        2.4023e-01};
  inline const static float coff_b[] = {4.6072e-01, -1.9199e+00, 2.5792e+00,
                                        -1.1200e+00};
  // methods
  void assignColor(float &r, float &g, float &b, float intensity);
};

void WistiaColormap::assignColor(float &r, float &g, float &b,
                                 float intensity) {
  r = polyValue(intensity, this->coff_r, 4U);
  g = polyValue(intensity, this->coff_g, 4U);
  b = polyValue(intensity, this->coff_b, 4U);
}

enum class DivergingColormap : unsigned char { Spectral, CoolWarm, Viridis };
class SpectralColormap {
 public:
  inline const static float coff_r[] = {6.4498e-01, 1.1186e+00, 4.1186e+00,
                                        -1.4378e+01, 8.8443e+00};
  inline const static float coff_g[] = {3.2065e-02, 1.7566e+00, 4.2265e+00,
                                        -1.0415e+01, 4.7360e+00};
  inline const static float coff_b[] = {2.6482e-01, -6.5753e-01, 6.5861e+00,
                                        -9.8745e+00, 4.3289e+00};
  // methods
  void assignColor(float &r, float &g, float &b, float intensity);
};

void SpectralColormap::assignColor(float &r, float &g, float &b,
                                   float intensity) {
  r = polyValue(intensity, this->coff_r, 5U);
  g = polyValue(intensity, this->coff_g, 5U);
  b = polyValue(intensity, this->coff_b, 5U);
}

class CoolWarmColormap {
 public:
  inline const static float coff_r[] = {2.4352e-01, 1.9864e+00, -1.5066e+00};
  inline const static float coff_g[] = {3.1919e-01, 2.2997e+00, -2.5500e+00};
  inline const static float coff_b[] = {7.7167e-01, 9.1354e-01, -1.5222e+00};
  // methods
  void assignColor(float &r, float &g, float &b, float intensity);
};

void CoolWarmColormap::assignColor(float &r, float &g, float &b,
                                   float intensity) {
  r = polyValue(intensity, this->coff_r, 3U);
  g = polyValue(intensity, this->coff_g, 3U);
  b = polyValue(intensity, this->coff_b, 3U);
}

class ViridisColormap {
 public:
  inline const static float coff_r[] = {0.27130491f, 0.81659759f, -6.19363115f,
                                        9.76791191f, -3.69728915f};
  ;
  inline const static float coff_g[] = {0.019942f, 1.51977397f, -1.60202556f,
                                        2.03930642f, -1.07467385f};
  ;
  inline const static float coff_b[] = {0.34726911f, 1.05768234f, -0.52648131f,
                                        -2.21724279f, 1.46271341f};
  ;
  // methods
  void assignColor(float &r, float &g, float &b, float intensity);
};

void ViridisColormap::assignColor(float &r, float &g, float &b,
                                  float intensity) {
  r = polyValue(intensity, this->coff_r, 5U);
  g = polyValue(intensity, this->coff_g, 5U);
  b = polyValue(intensity, this->coff_b, 5U);
}

}  // namespace colormap

#endif

#ifndef BOTALAB_CYCLIC_COLORMAP_HPP
#define BOTALAB_CYCLIC_COLORMAP_HPP
#include <Arduino.h>
#include <math.h>

namespace colormap::cyclic {

class SineWaveParam {
   private:
    float amp_;
    float freq_;
    float initial_phase_;
    float dc_offset_;

   public:
    SineWaveParam(/* args */);
    SineWaveParam(float amp, float freq, float initial_phase, float dc_offset)
        : amp_(amp),
          freq_(freq),
          initial_phase_(initial_phase),
          dc_offset_(dc_offset) {}
    // ~SineWaveParam();

    float amp() { return amp_; }
    float freq() { return freq_; }
    float initial_phase() { return initial_phase_; }
    float dc_offset() { return dc_offset_; }
};
SineWaveParam::SineWaveParam(/* args */) {
    SineWaveParam(0.5f, 1.0f, 0.0f, 0.5f);
};

class RgbSineParam {
   private:
    SineWaveParam r_;
    SineWaveParam g_;
    SineWaveParam b_;

   public:
    RgbSineParam(/* args */);
    RgbSineParam(SineWaveParam r, SineWaveParam g, SineWaveParam b)
        : r_(r), g_(g), b_(b) {}
    // ~RgbSineParam();

    SineWaveParam* r() { return &r_; }
    SineWaveParam* g() { return &g_; }
    SineWaveParam* b() { return &b_; }
};

RgbSineParam::RgbSineParam(/* args */) {
    RgbSineParam(SineWaveParam(), SineWaveParam(), SineWaveParam());
}

// presets
RgbSineParam HSV = RgbSineParam(
    SineWaveParam(0.5f, 1.0f, M_PI * 0.5f, 0.5f),
    SineWaveParam(0.5f, 1.0f, M_PI * 0.5f - 2.0f * M_PI / 3.0f, 0.5f),
    SineWaveParam(0.5f, 1.0f, M_PI * 0.5f - 4.0f * M_PI / 3.0f, 0.5f));

RgbSineParam FLARE =
    RgbSineParam(SineWaveParam(-0.008f, 1.000f, 1.571f, 0.953f),
                 SineWaveParam(-0.267f, 1.000f, 1.571f, 0.420f),
                 SineWaveParam(-0.008f, 1.000f, 1.571f, 0.082f));
RgbSineParam PEACH =
    RgbSineParam(SineWaveParam(-0.035f, 1.000f, 1.571f, 0.965f),
                 SineWaveParam(-0.335f, 1.000f, 1.571f, 0.594f),
                 SineWaveParam(-0.035f, 1.000f, 1.571f, 0.565f));
RgbSineParam PURPINK =
    RgbSineParam(SineWaveParam(-0.192f, 1.000f, 1.571f, 0.690f),
                 SineWaveParam(0.000f, 1.000f, 1.571f, 0.000f),
                 SineWaveParam(-0.192f, 1.000f, 1.571f, 1.000f));
RgbSineParam RAINBOW_BLUE =
    RgbSineParam(SineWaveParam(-0.010f, 1.000f, 1.571f, 0.010f),
                 SineWaveParam(0.245f, 1.000f, 1.571f, 0.704f),
                 SineWaveParam(-0.010f, 1.000f, 1.571f, 0.639f));
RgbSineParam SUBLIME_VIVID =
    RgbSineParam(SineWaveParam(0.371f, 1.000f, 1.571f, 0.618f),
                 SineWaveParam(-0.047f, 1.000f, 1.571f, 0.322f),
                 SineWaveParam(0.371f, 1.000f, 1.571f, 0.702f));
RgbSineParam TIMBER =
    RgbSineParam(SineWaveParam(0.494f, 1.000f, 1.571f, 0.494f),
                 SineWaveParam(-0.429f, 1.000f, 1.571f, 0.429f),
                 SineWaveParam(0.494f, 1.000f, 1.571f, 0.935f));

RgbSineParam VENICE =
    RgbSineParam(SineWaveParam(-0.137f, 1.000f, 1.571f, 0.518f),
                 SineWaveParam(-0.092f, 1.000f, 1.571f, 0.657f),
                 SineWaveParam(-0.137f, 1.000f, 1.571f, 0.910f));

// end of presets
float cyclicValue(float intensity, SineWaveParam& param) {
    return param.dc_offset() +
           param.amp() * sinf(2.0f * M_PI * param.freq() * intensity +
                              param.initial_phase());
}

void assignCyclicColor(float& r, float& g, float& b, float intensity,
                       RgbSineParam& param) {
    r = cyclicValue(intensity, *param.r());
    g = cyclicValue(intensity, *param.g());
    b = cyclicValue(intensity, *param.b());

    r = constrain(r, 0.0f, 1.0f);
    g = constrain(g, 0.0f, 1.0f);
    b = constrain(b, 0.0f, 1.0f);
}
}  // namespace colormap::cyclic
#endif
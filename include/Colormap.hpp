#ifndef BOTALAB_COLORMAP_HPP
#define BOTALAB_COLORMAP_HPP
#include <Arduino.h>
#include <math.h>
namespace colormap {

float polyValue(float x, const float coffs[], size_t coffs_length) {
    float y = 0.0f;
    for (size_t i = 0; i < coffs_length; i++) {
        if (i == 0) {
            y += coffs[i];
        } else {
            y += coffs[i] * powf(x, i);
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

enum class ColormapId : unsigned char {
    // cyclic
    Hsv,
    Twilight,
    TwilightShifted,
    // sequential
    Cool,
    Hot,
    Wistia,
    // diverging
    Spectral,
    CoolWarm,
    Viridis,
    // custom
    Instagram,
    Martini,
    Megatoron,
    LENGTH
};

String colormap_name(ColormapId cmap_id) {
    static String names[] = {
        "Hsv",     "Twilight",  "TwilightS", "Cool",
        "Hot",     "Wistia",    "Spectral",  "CoolWarm",
        "Viridis", "Instagram", "Martini",   "Megatoron",
    };
    return names[static_cast<uint8_t>(cmap_id)];
}

void assignHsvHueColor(float &r, float &g, float &b, float intensity,
                       float initial_phase) {
    static const float r_a0 = 4.8585e-01f;
    static const float r_a[] = {6.0380e-01f, -1.2575e-01f};
    static const float r_b[] = {-1.0742e-03, 9.5391e-04};
    static const float r_f[] = {9.8438e-01, 2.9531e+00};

    static const float g_a0 = 5.1379e-01f;
    static const float g_a[] = {-2.9102e-01f, -1.3128e-01f};
    static const float g_b[] = {-5.3190e-01, -9.5479e-03};
    static const float g_f[] = {9.8438e-01, 2.9531e+00};

    static const float b_a0 = 5.1249e-01;
    static const float b_a[] = {-2.9224e-01, -1.3310e-01};
    static const float b_b[] = {5.3225e-01, 1.0011e-02};
    static const float b_f[] = {9.8438e-01, 2.9531e+00};

    r = cyclicValue(intensity, initial_phase, r_a0, r_a, r_b, r_f, 2U);
    g = cyclicValue(intensity, initial_phase, g_a0, g_a, g_b, g_f, 2U);
    b = cyclicValue(intensity, initial_phase, b_a0, b_a, b_b, b_f, 2U);

    r = constrain(r, 0.0f, 1.0f);
    g = constrain(g, 0.0f, 1.0f);
    b = constrain(b, 0.0f, 1.0f);
}

void assignTwilightColor(float &r, float &g, float &b, float intensity,
                         float initial_phase) {
    static const float r_a0 = 5.3852e-01f;
    static const float r_a[] = {2.5764e-01, 5.9289e-02};
    static const float r_b[] = {1.4902e-01, -1.3312e-02};
    static const float r_f[] = {9.8438e-01, 2.9531e+00};

    static const float g_a0 = 4.1168e-01f;
    static const float g_a[] = {3.8150e-01, 3.0205e-02};
    static const float g_b[] = {-3.3706e-02, -3.1774e-03};
    static const float g_f[] = {9.8438e-01, 1.9688e+00};

    static const float b_a0 = 5.3664e-01;
    static const float b_a[] = {2.4245e-01, 6.7400e-02};
    static const float b_b[] = {-1.7885e-01, 2.8135e-02};
    static const float b_f[] = {9.8438e-01, 2.9531e+00};

    r = cyclicValue(intensity, initial_phase, r_a0, r_a, r_b, r_f, 2U);
    g = cyclicValue(intensity, initial_phase, g_a0, g_a, g_b, g_f, 2U);
    b = cyclicValue(intensity, initial_phase, b_a0, b_a, b_b, b_f, 2U);

    r = constrain(r, 0.0f, 1.0f);
    g = constrain(g, 0.0f, 1.0f);
    b = constrain(b, 0.0f, 1.0f);
}

void assignTwilightShiftedColor(float &r, float &g, float &b, float intensity,
                                float initial_phase) {
    static const float r_a0 = 5.4938e-01f;
    static const float r_a[] = {-2.7070e-01, -5.1723e-02};
    static const float r_b[] = {1.2390e-01, -3.2384e-02};
    static const float r_f[] = {9.8438e-01, 2.9531e+00};

    static const float g_a0 = 4.2379e-01;
    static const float g_a[] = {-3.7971e-01, 2.2300e-02};
    static const float g_b[] = {-7.1732e-02, 5.3167e-03};
    static const float g_f[] = {9.8438e-01, 1.9688e+00};

    static const float b_a0 = 5.4709e-01;
    static const float b_a[] = {-2.2441e-01, -7.1713e-02};
    static const float b_b[] = {-2.0186e-01, 7.4686e-03};
    static const float b_f[] = {9.8438e-01, 2.9531e+00};

    r = cyclicValue(intensity, initial_phase, r_a0, r_a, r_b, r_f, 2U);
    g = cyclicValue(intensity, initial_phase, g_a0, g_a, g_b, g_f, 2U);
    b = cyclicValue(intensity, initial_phase, b_a0, b_a, b_b, b_f, 2U);

    r = constrain(r, 0.0f, 1.0f);
    g = constrain(g, 0.0f, 1.0f);
    b = constrain(b, 0.0f, 1.0f);
}

enum class SequentialColormap : unsigned char { Cool, Hot, Wistia };

void assignCoolColor(float &r, float &g, float &b, float intensity) {
    static const float coff_r[] = {1.1765e-02, 9.7647e-01};
    static const float coff_g[] = {9.8824e-01, -9.7647e-01};
    static const float coff_b[] = {1.0000e+00, -7.3651e-17};
    r = polyValue(intensity, coff_r, 2U);
    g = polyValue(intensity, coff_g, 2U);
    b = polyValue(intensity, coff_b, 2U);

    r = constrain(r, 0.0f, 1.0f);
    g = constrain(g, 0.0f, 1.0f);
    b = constrain(b, 0.0f, 1.0f);
}

void assignHotColor(float &r, float &g, float &b, float intensity) {
    static const float coff_r[] = {7.2484e-02, 3.0686e+00, -9.9480e-01,
                                   -4.2600e+00, 3.1138e+00};
    static const float coff_g[] = {1.9271e-07, -5.2787e-01, 6.9576e-01,
                                   6.2264e+00, -5.3943e+00};
    static const float coff_b[] = {-5.5203e-08, 8.4880e-02, 2.9603e-01,
                                   -2.8484e+00, 3.4211e+00};
    r = polyValue(intensity, coff_r, 5U);
    g = polyValue(intensity, coff_g, 5U);
    b = polyValue(intensity, coff_b, 5U);

    r = constrain(r, 0.0f, 1.0f);
    g = constrain(g, 0.0f, 1.0f);
    b = constrain(b, 0.0f, 1.0f);
}

void assignWistiaColor(float &r, float &g, float &b, float intensity) {
    static const float coff_r[] = {8.9910e-01, 5.3242e-01, -8.2493e-01,
                                   3.8220e-01};
    static const float coff_g[] = {9.9576e-01, -3.3824e-01, -3.9361e-01,
                                   2.4023e-01};
    static const float coff_b[] = {4.6072e-01, -1.9199e+00, 2.5792e+00,
                                   -1.1200e+00};
    r = polyValue(intensity, coff_r, 5U);
    g = polyValue(intensity, coff_g, 5U);
    b = polyValue(intensity, coff_b, 5U);

    r = constrain(r, 0.0f, 1.0f);
    g = constrain(g, 0.0f, 1.0f);
    b = constrain(b, 0.0f, 1.0f);
}

void assignSpectralColor(float &r, float &g, float &b, float intensity) {
    static const float coff_r[] = {6.4498e-01, 1.1186e+00, 4.1186e+00,
                                   -1.4378e+01, 8.8443e+00};
    static const float coff_g[] = {3.2065e-02, 1.7566e+00, 4.2265e+00,
                                   -1.0415e+01, 4.7360e+00};
    static const float coff_b[] = {2.6482e-01, -6.5753e-01, 6.5861e+00,
                                   -9.8745e+00, 4.3289e+00};
    r = polyValue(intensity, coff_r, 5U);
    g = polyValue(intensity, coff_g, 5U);
    b = polyValue(intensity, coff_b, 5U);

    r = constrain(r, 0.0f, 1.0f);
    g = constrain(g, 0.0f, 1.0f);
    b = constrain(b, 0.0f, 1.0f);
}
void assignCoolWarmColor(float &r, float &g, float &b, float intensity) {
    static const float coff_r[] = {2.4352e-01, 1.9864e+00, -1.5066e+00};
    static const float coff_g[] = {3.1919e-01, 2.2997e+00, -2.5500e+00};
    static const float coff_b[] = {7.7167e-01, 9.1354e-01, -1.5222e+00};

    r = polyValue(intensity, coff_r, 3U);
    g = polyValue(intensity, coff_g, 3U);
    b = polyValue(intensity, coff_b, 3U);

    r = constrain(r, 0.0f, 1.0f);
    g = constrain(g, 0.0f, 1.0f);
    b = constrain(b, 0.0f, 1.0f);
}

void assignViridisColor(float &r, float &g, float &b, float intensity) {
    static const float coff_r[] = {0.27130491f, 0.81659759f, -6.19363115f,
                                   9.76791191f, -3.69728915f};

    static const float coff_g[] = {0.019942f, 1.51977397f, -1.60202556f,
                                   2.03930642f, -1.07467385f};

    static const float coff_b[] = {0.34726911f, 1.05768234f, -0.52648131f,
                                   -2.21724279f, 1.46271341f};

    r = polyValue(intensity, coff_r, 5U);
    g = polyValue(intensity, coff_g, 5U);
    b = polyValue(intensity, coff_b, 5U);

    r = constrain(r, 0.0f, 1.0f);
    g = constrain(g, 0.0f, 1.0f);
    b = constrain(b, 0.0f, 1.0f);
}

void assignInstagramColor(float &r, float &g, float &b, float intensity) {
    static const float coff_r[] = {0.5137255337955772f, 0.6674700178273065f,
                                   2.3996448417985077f, -5.172947641644774f,
                                   2.5803425940614506f};

    static const float coff_g[] = {
        0.22745090839887416f, 0.17150524455463953f, -3.387647901303438f,
        7.371526042362648f,   -3.692638280364719f,
    };

    static const float coff_b[] = {0.7058822827005238, -0.7389287050284381,
                                   -3.71413099481592, 8.02475544214574,
                                   -4.006989867585645};

    r = polyValue(intensity, coff_r, 5U);
    g = polyValue(intensity, coff_g, 5U);
    b = polyValue(intensity, coff_b, 5U);

    r = constrain(r, 0.0f, 1.0f);
    g = constrain(g, 0.0f, 1.0f);
    b = constrain(b, 0.0f, 1.0f);
}

void assignMartiniColor(float &r, float &g, float &b, float intensity) {
    static const float coff_r[] = {0.9921569538692818, -0.4881775295463269,
                                   4.165519438685839, -9.081285726704252,
                                   4.5529634118932};

    static const float coff_g[] = {0.32941182332642055, 0.9267362737812725,
                                   3.2396033218957334, -6.982076358102499,
                                   3.4824034404853896};

    static const float coff_b[] = {0.9137254353788334, -0.9027979042878135,
                                   -3.045748296079152, 6.562330284608044,
                                   -3.27260762489274};

    r = polyValue(intensity, coff_r, 5U);
    g = polyValue(intensity, coff_g, 5U);
    b = polyValue(intensity, coff_b, 5U);

    r = constrain(r, 0.0f, 1.0f);
    g = constrain(g, 0.0f, 1.0f);
    b = constrain(b, 0.0f, 1.0f);
}

void assignMegatoronColor(float &r, float &g, float &b, float intensity) {
    static const float coff_r[] = {0.7764706086733416, 0.28194513294851825,
                                   1.1109132084820332, -2.396480702488311,
                                   1.1957792271087373};

    static const float coff_g[] = {1.0000000253159125, -0.43104883148598716,
                                   1.023943509616419, -2.251501496314707,
                                   1.1331166135685438};

    static const float coff_b[] = {0.8666666396540221, -0.49783398198512796,
                                   -1.5247697527185393, 3.282418785996648,
                                   -1.6362856459243542};

    r = polyValue(intensity, coff_r, 5U);
    g = polyValue(intensity, coff_g, 5U);
    b = polyValue(intensity, coff_b, 5U);

    r = constrain(r, 0.0f, 1.0f);
    g = constrain(g, 0.0f, 1.0f);
    b = constrain(b, 0.0f, 1.0f);
}

// utils

void assignCyclicColor(float &r, float &g, float &b, float intensity,
                       float initial_phase, ColormapId cmap) {
    switch (cmap) {
        case ColormapId::Hsv:
            assignHsvHueColor(r, g, b, intensity, initial_phase);
            break;
        case ColormapId::Twilight:
            assignTwilightColor(r, g, b, intensity, initial_phase);
            break;
        case ColormapId::TwilightShifted:
            assignTwilightShiftedColor(r, g, b, intensity, initial_phase);
            break;
        default:
            break;
    }
    return;
}

void assignSequentialColor(float &r, float &g, float &b, float intensity,
                           ColormapId cmap) {
    switch (cmap) {
        case ColormapId::Cool:
            assignCoolColor(r, g, b, intensity);
            break;
        case ColormapId::Hot:
            assignHotColor(r, g, b, intensity);
            break;
        case ColormapId::Wistia:
            assignWistiaColor(r, g, b, intensity);
            break;

        default:
            break;
    }
    return;
}

void assignDivergingColor(float &r, float &g, float &b, float intensity,
                          ColormapId cmap) {
    switch (cmap) {
        case ColormapId::Spectral:
            assignSpectralColor(r, g, b, intensity);
            break;
        case ColormapId::CoolWarm:
            assignCoolWarmColor(r, g, b, intensity);
            break;
        case ColormapId::Viridis:
            assignViridisColor(r, g, b, intensity);
            break;
        case ColormapId::Instagram:
            assignInstagramColor(r, g, b, intensity);
            break;
        case ColormapId::Martini:
            assignMartiniColor(r, g, b, intensity);
            break;
        case ColormapId::Megatoron:
            assignMegatoronColor(r, g, b, intensity);
            break;
        default:
            break;
    }
    return;
}

void assignColor(float &r, float &g, float &b, float intensity, ColormapId cmap,
                 float initial_phase) {
    switch (cmap) {
        case ColormapId::Hsv:
        case ColormapId::Twilight:
        case ColormapId::TwilightShifted:
            assignCyclicColor(r, g, b, intensity, initial_phase, cmap);
            break;
        case ColormapId::Cool:
        case ColormapId::Hot:
        case ColormapId::Wistia:
            assignSequentialColor(r, g, b, intensity, cmap);
            break;
        case ColormapId::Spectral:
        case ColormapId::CoolWarm:
        case ColormapId::Viridis:
        case ColormapId::Instagram:
        case ColormapId::Martini:
        case ColormapId::Megatoron:
            assignDivergingColor(r, g, b, intensity, cmap);
            break;
        default:
            break;
    }
}

}  // namespace colormap

#endif
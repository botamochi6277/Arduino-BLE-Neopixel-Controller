#ifndef BOTALAB_COLORMAP_HPP
#define BOTALAB_COLORMAP_HPP
#include <Arduino.h>
#include <math.h>

#include <map>

#include "CyclicColormap.hpp"
#include "DivergingColormap.hpp"
namespace colormap {

// https://google.github.io/styleguide/cppguide.html#Enumerator_Names
enum class ColormapId : unsigned char {
    // cyclic
    kHsv,
    kFlare,
    kPeach,
    kPurpink,
    kRainbowBlue,
    kSublimeVivid,
    kTimber,
    kVenice,
    // diverging
    kArgon,
    kJShine,
    kRelay,
    kCoolWarm,
    kBackToEarth,
    kIris,
    // custom
    LENGTH
};

String colormap_name(ColormapId cmap_id) {
    static String names[] = {
        // cyclic
        "Hsv",
        "Flare",
        "Peach",
        "Purpink",
        "RainbowBlue",
        "SublimeVivid",
        "Timber",
        "Venice",
        // diverging
        "Argon",
        "JShine",
        "Relay",
        "CoolWarm",
        "BackToEarth",
        "Iris",
    };
    return names[static_cast<uint8_t>(cmap_id)];
}

void assignColorWithColormap(float &r, float &g, float &b, float intensity,
                             ColormapId cmap) {
    switch (cmap) {
        // cyclic
        case ColormapId::kHsv:
            cyclic::assignCyclicColor(r, g, b, intensity, cyclic::HSV);
            break;
        case ColormapId::kFlare:
            cyclic::assignCyclicColor(r, g, b, intensity, cyclic::FLARE);
            break;
        case ColormapId::kPeach:
            cyclic::assignCyclicColor(r, g, b, intensity, cyclic::PEACH);
            break;
        case ColormapId::kPurpink:
            cyclic::assignCyclicColor(r, g, b, intensity, cyclic::PURPINK);
            break;
        case ColormapId::kRainbowBlue:
            cyclic::assignCyclicColor(r, g, b, intensity, cyclic::RAINBOW_BLUE);
            break;
        case ColormapId::kSublimeVivid:
            cyclic::assignCyclicColor(r, g, b, intensity,
                                      cyclic::SUBLIME_VIVID);
            break;
        case ColormapId::kTimber:
            cyclic::assignCyclicColor(r, g, b, intensity, cyclic::TIMBER);
            break;
        case ColormapId::kVenice:
            cyclic::assignCyclicColor(r, g, b, intensity, cyclic::VENICE);
            break;
        // diverging
        case ColormapId::kArgon:
            diverging::assignPolyColor(r, g, b, intensity, diverging::ARGON);
            break;
        case ColormapId::kJShine:
            diverging::assignPolyColor(r, g, b, intensity, diverging::JSHINE);
            break;
        case ColormapId::kRelay:
            diverging::assignPolyColor(r, g, b, intensity, diverging::RELAY);
            break;
        case ColormapId::kCoolWarm:
            diverging::assignPolyColor(r, g, b, intensity, diverging::COOLWARM);
            break;
        case ColormapId::kBackToEarth:
            diverging::assignPolyColor(r, g, b, intensity,
                                       diverging::BACK_TO_EARTH);
            break;
        case ColormapId::kIris:
            diverging::assignPolyColor(r, g, b, intensity, diverging::IRIS);
            break;
        default:
            break;
    }
    return;
}

}  // namespace colormap

#endif
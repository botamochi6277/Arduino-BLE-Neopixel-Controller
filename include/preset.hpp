#ifndef PRESET_HPP
#define PRESET_HPP

#define PRESET_OFF 0x0
#define PRESET_PRIMARY 0x1
#define PRESET_SECONDARY 0x2
#define PRESET_ERROR 0x3
#define PRESET_WARNING 0x4
#define PRESET_INFO 0x5
#define PRESET_SUCCESS 0x6
#define PRESET_LIGHT 0x7
#define PRESET_DARK 0x8

// action code
#define PRESET_ACT_WIPE 0x1
#define PRESET_ACT_SLIDE 0x2
#define PRESET_ACT_DISSOLVE 0x3
#define PRESET_ACT_ACC 0x4

// action option
#define PRESET_OPT_DIS 0x01      // with dissolve
#define PRESET_OPT_INV 0x02      // inverse (backward)
#define PRESET_OPT_DIS_INV 0x03  // inverse with dissolve

// Standard Palette
// https://mui.com/material-ui/customization/palette/
#define PALETTE_PRIMARY 0x90caf9
#define PALETTE_SECONDARY 0xce93d8
#define PALETTE_ERROR 0xf44336
#define PALETTE_WARNING 0xffa726
#define PALETTE_INFO 0x29b6f6
#define PALETTE_SUCCESS 0x66bb6a

// https://coolors.co/fb5607-ffbe0b-99ff40-4ec778-39ede4-3a86ff-8338ec-eb52bd-ff006e
#define PALETTE_LIGHT 0xffffff

#define PALETTE_ORANGE 0xFB5607   // h 019
#define PALETTE_YELLOW 0xFFBE0B   // h 044
#define PALETTE_LIME 0x99FF40     // h 092
#define PALETTE_GREEN 0x44AF69    // h 141
#define PALETTE_AQUA 0x39EDE4     // h 177
#define PALETTE_BLUE 0x3A86FF     // h 217
#define PALETTE_VIOLET 0x8338EC   // h 265
#define PALETTE_MAGENTA 0xEB52BD  // h 318
#define PALETTE_RED 0xFF006E      // h 334

const unsigned int PALETTE[] = {
    0,                // 0
    PALETTE_LIGHT,    // 1
    PALETTE_ORANGE,   // 2
    PALETTE_YELLOW,   // 3
    PALETTE_LIME,     // 4
    PALETTE_GREEN,    // 5
    PALETTE_AQUA,     // 6
    PALETTE_BLUE,     // 7
    PALETTE_VIOLET,   // 8
    PALETTE_MAGENTA,  // 9
    PALETTE_RED       // A
};

#endif
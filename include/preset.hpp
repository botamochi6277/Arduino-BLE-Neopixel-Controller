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

#define PRESET_ACT_WIPE 0x1
#define PRESET_ACT_SLIDE 0x2
#define PRESET_ACT_DISSOLVE 0x03

// Standard Palette
// https://mui.com/material-ui/customization/palette/
#define PALETTE_PRIMARY 0x90caf9
#define PALETTE_SECONDARY 0xce93d8
#define PALETTE_ERROR 0xf44336
#define PALETTE_WARNING 0xffa726
#define PALETTE_INFO 0x29b6f6
#define PALETTE_SUCCESS 0x66bb6a
#define PALETTE_LIGHT 0xffffff
#define PALETTE_DARK 0x2b2b2b

const unsigned int PALETTE[] = {
    0,
    PALETTE_PRIMARY,
    PALETTE_SECONDARY,
    PALETTE_ERROR,
    PALETTE_WARNING,
    PALETTE_INFO,
    PALETTE_SUCCESS,
    PALETTE_LIGHT,
    PALETTE_DARK,
};

#endif
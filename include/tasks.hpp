#ifndef BOTALAB_TASKS_HPP
#define BOTALAB_TASKS_HPP

#include <Adafruit_NeoPixel.h>
#include <TaskManager.h>

#include "neopixel_service.hpp"
#include "strip.hpp"

namespace tasks {

void updateColorCache(led_strip::PixelManager &manager,
                      ble::NeoPixelService &pixel_srv,
                      Adafruit_NeoPixel &pixels) {
  static unsigned int loop_count = 0;
  if (pixel_srv.brightness_chr.written()) {
    pixels.setBrightness(pixel_srv.brightness_chr.value());
  }

  // color parameter was changed, updating static colors
  if (pixel_srv.color01_chr.written() || pixel_srv.color02_chr.written() ||
      pixel_srv.color03_chr.written() || pixel_srv.color04_chr.written() ||
      pixel_srv.blending_chr.written() || pixel_srv.num_colors_chr.written() ||
      loop_count == 0) {
    // update clock

    // assign palette colors
    manager.setPaletteColor(0, pixel_srv.color01_chr.value());
    manager.setPaletteColor(1, pixel_srv.color02_chr.value());
    manager.setPaletteColor(2, pixel_srv.color03_chr.value());
    manager.setPaletteColor(3, pixel_srv.color04_chr.value());

    // led_strip::blend(transited_colors, palette, pixels.numPixels(),
    //                  pixel_srv.num_colors_chr.value(),
    //                  pixel_srv.blending_chr.value());
  }
}

void setRainbow(led_strip::PixelManager &manager, Adafruit_NeoPixel &pixels,
                float time, float period) {
  for (size_t i = 0; i < NUM_PIXELS; i++) {
    manager.pixel_units[i].setHueCycle(time, period,
                                       i * (2.0f * M_PI / (NUM_PIXELS - 1)));
  }
}

void setPixelColors(led_strip::PixelManager &manager,
                    Adafruit_NeoPixel &pixels) {
  for (uint16_t i = 0; i < pixels.numPixels(); i++) {
    pixels.setPixelColor(i, manager.pixel_units[i].hex());
  }
}

// void updatePixelColors

}  // namespace tasks

#endif
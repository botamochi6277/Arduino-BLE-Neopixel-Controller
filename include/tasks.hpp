#ifndef BOTALAB_TASKS_HPP
#define BOTALAB_TASKS_HPP

#include <Adafruit_NeoPixel.h>
#include <TaskManager.h>

#include "Colormap.hpp"
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
      pixel_srv.colormap_chr.written() || pixel_srv.num_colors_chr.written() ||
      loop_count == 0) {
    // update clock

    // assign palette colors
    manager.setPaletteColor(0, pixel_srv.color01_chr.value());
    manager.setPaletteColor(1, pixel_srv.color02_chr.value());
    manager.setPaletteColor(2, pixel_srv.color03_chr.value());
    manager.setPaletteColor(3, pixel_srv.color04_chr.value());

    // led_strip::blend(transited_colors, palette, pixels.numPixels(),
    //                  pixel_srv.num_colors_chr.value(),
    //                  pixel_srv.colormap_chr.value());
  }
}

// void setHeatColors(led_strip::PixelManager &manager, float temperature) {
//   for (uint16_t i = 0; i < NUM_PIXELS; i++) {
//     manager.pixel_units[i].setHeatColor(temperature);
//   }
// }

// void updatePeriodicColors(led_strip::PixelManager &manager, uint8_t colormap,
//                           float time, float freq, float wave_length) {
//   // freq
//   // wavelength
//   // ξ＝a sin {ω(t－x/v)＋ε}
//   for (uint16_t i = 0; i < NUM_PIXELS; i++) {
//     float initial_phase = 2.0f * M_PI * i / NUM_PIXELS;
//     manager.pixel_units[i].setPeriodicColor(
//         time, 1.0 / freq, initial_phase,
//         static_cast<colormap::CyclicColormap>(colormap));
//     // manager.pixel_units[i].setHeatColor(temperature);
//   }
// }

void setPixelColors(led_strip::PixelManager &manager,
                    Adafruit_NeoPixel &pixels) {
  for (uint16_t i = 0; i < pixels.numPixels(); i++) {
    pixels.setPixelColor(i, manager.pixel_units[i].hex());
  }
}

enum class SensorSource { Timer, ACC };

void updatePixelColors(led_strip::PixelManager &manager, SensorSource sensor,
                       led_strip::IntensityFuncId func_id,
                       colormap::ColormapId cmap) {
  // read sensor value
  float sensor_value = 0.0f;
  switch (sensor) {
    case SensorSource::Timer:
      sensor_value = millis() / 1.0e3f;
      break;

    default:
      break;
  }
  // compute intensity
  manager.setIntensity(sensor_value, func_id);
  manager.setColor(cmap);
}

}  // namespace tasks

#endif
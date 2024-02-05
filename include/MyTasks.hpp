#ifndef BOTALAB_TASKS_HPP
#define BOTALAB_TASKS_HPP

#include <Adafruit_NeoPixel.h>
#include <TaskManager.h>

#include "Colormap.hpp"
#include "LedStrip.hpp"
#include "NeopixelService.hpp"

namespace tasks {

void reflectParams(led_strip::PixelManager &manager,
                   ble::NeoPixelService &pixel_srv, Adafruit_NeoPixel &pixels,
                   bool is_on_written = true) {
  static unsigned int loop_count = 0;
  if (!is_on_written | pixel_srv.brightness_chr.written()) {
    pixels.setBrightness(pixel_srv.brightness_chr.value());
  }
  if (!is_on_written | pixel_srv.colormap_chr.written()) {
    manager.setColormap(pixel_srv.colormap_chr.value());
    // pixel_srv.colormap_name_chr.writeValue(
    //     colormap::colormap_name(manager.colormap()));
  }
  if (!is_on_written | pixel_srv.intensity_func_chr.written()) {
    manager.setIntensityFuncId(pixel_srv.intensity_func_chr.value());
    // (todo) assign func name
  }

  if (!is_on_written | pixel_srv.wave_width_chr.written()) {
    manager.setWaveWidth(pixel_srv.wave_width_chr.valueLE());
  }
  if (!is_on_written | pixel_srv.wave_freq_chr.written()) {
    manager.setWaveFreq(pixel_srv.wave_freq_chr.valueLE());
  }
  if (!is_on_written | pixel_srv.wave_speed_chr.written()) {
    manager.setWaveSpeed(pixel_srv.wave_speed_chr.valueLE());
  }

  loop_count += 1;
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

enum class SensorSource : unsigned char { Time, Cycle, ACC };

void updatePixelColors(led_strip::PixelManager &manager, SensorSource sensor) {
  // read sensor value
  float sensor_value = 0.0f;
  switch (sensor) {
    case SensorSource::Time:
      sensor_value = millis() / 1.0e3f;
      break;
    default:
      break;
  }
  // compute intensity
  manager.computeAndSetIntensity(sensor_value);
  manager.setColor();
}

}  // namespace tasks

#endif
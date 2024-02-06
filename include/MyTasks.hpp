#ifndef BOTALAB_TASKS_HPP
#define BOTALAB_TASKS_HPP

#include <Adafruit_NeoPixel.h>
#include <TaskManager.h>
#ifdef LSM6DS3_ENABLED
#include "LSM6DS3.h"  // IMU chip
#endif

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

void setPixelColors(led_strip::PixelManager &manager,
                    Adafruit_NeoPixel &pixels) {
  for (uint16_t i = 0; i < pixels.numPixels(); i++) {
    pixels.setPixelColor(i, manager.pixel_units[i].hex());
  }
}

enum class InputSource : unsigned char {
  Time,
  AccX,
  AccY,
  AccZ,
  AccMAG,
  GyroX,
  GyroY,
  GyroZ
};

void updatePixelColors(led_strip::PixelManager &manager,
                       InputSource input_src) {
  // read sensor value
  float sensor_value = 0.0f;
  switch (input_src) {
    case InputSource::Time:
      sensor_value = millis() / 1.0e3f;
      break;
    default:
      break;
  }
  // compute intensity
  manager.computeAndSetIntensity(sensor_value);
  manager.setColor();
}
#ifdef LSM6DS3_ENABLED
void updatePixelColors(led_strip::PixelManager &manager, LSM6DS3 &imu,
                       InputSource input_src) {
  // read sensor value
  float sensor_value = 0.0f;
  switch (input_src) {
    case InputSource::Time:
      sensor_value = millis() / 1.0e3f;
      break;
    case InputSource::AccX:
      sensor_value =
          easing::remap(imu.readFloatAccelX(), -2.0f, 2.0f, -1.0f, 1.0f);
      break;
    case InputSource::AccY:
      sensor_value =
          easing::remap(imu.readFloatAccelY(), -2.0f, 2.0f, -1.0f, 1.0f);
      break;
    case InputSource::AccZ:
      sensor_value =
          easing::remap(imu.readFloatAccelZ(), -2.0f, 2.0f, -1.0f, 1.0f);
      break;
    case InputSource::GyroX:
      sensor_value =
          easing::remap(imu.readFloatGyroX(), -180.0f, 180.0f, -1.0f, 1.0f);
      break;
    case InputSource::GyroY:
      sensor_value =
          easing::remap(imu.readFloatGyroY(), -180.0f, 180.0f, -1.0f, 1.0f);
      break;
    case InputSource::GyroZ:
      sensor_value =
          easing::remap(imu.readFloatGyroZ(), -180.0f, 180.0f, -1.0f, 1.0f);
      break;
    default:
      break;
  }
  // compute intensity
  manager.computeAndSetIntensity(sensor_value);
  manager.setColor();
}
#endif

}  // namespace tasks

#endif
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
// (todo) separate as DataSource.hpp
enum class DataSource : unsigned char {
  BeatSin05,  // sine beat 5Hz
  BeatSin10,
  BeatSin20,
  BeatSaw05,  // saw beat 5Hz
  BeatSaw10,  // saw beat 1.0Hz
  BeatSaw20,  // saw beat 2.0Hz
  AccX,
  AccY,
  AccZ,
  AccMAG,
  GyroX,
  GyroY,
  GyroZ,
  LENGTH
};
String input_name(DataSource id) {
  static String names[] = {
      "Beat/Sine/0.5Hz", "Beat/Sine/1.0Hz", "Beat/Sine/2.0Hz", "Beat/Saw/0.5Hz",
      "Beat/Saw/1.0Hz",  "Beat/Saw/2.0Hz",  "Acc/X",           "Acc/Y",
      "Acc/Z",           "Acc/Mag",         "Gyro/X",          "Gyro/Y",
      "Gyro/Z",
  };
  return names[static_cast<uint8_t>(id)];
}

void reflectParams(led_strip::PixelManager &manager,
                   ble::NeoPixelService &pixel_srv, Adafruit_NeoPixel &pixels,
                   bool is_on_written = true) {
  static unsigned int loop_count = 0;
  if (!is_on_written | pixel_srv.brightness_chr.written()) {
    pixels.setBrightness(pixel_srv.brightness_chr.value());
  }

  // colormap
  if (!is_on_written | pixel_srv.colormap_chr.written()) {
    if (pixel_srv.colormap_chr.value() <
        static_cast<uint8_t>(colormap::ColormapId::LENGTH)) {
      manager.setColormap(pixel_srv.colormap_chr.value());
      pixel_srv.colormap_name_chr.writeValue(
          colormap::colormap_name(manager.colormap()));
    }
  }

  // intensity function
  if (!is_on_written | pixel_srv.intensity_func_chr.written()) {
    if (pixel_srv.intensity_func_chr.value() <
        static_cast<uint8_t>(led_strip::IntensityFuncId::LENGTH)) {
      manager.setIntensityFuncId(pixel_srv.intensity_func_chr.value());
      pixel_srv.intensity_name_chr.writeValue(
          led_strip::intensity_func_name(manager.intensity_func_id()));
    }
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

  // data source
  if (!is_on_written | pixel_srv.input_chr.written()) {
    if (pixel_srv.input_chr.value() <
        static_cast<uint8_t>(DataSource::LENGTH)) {
      pixel_srv.input_name_chr.writeValue(
          input_name(static_cast<DataSource>(pixel_srv.input_chr.valueLE())));
    } else {
      /* out of index */
    }
  }

  loop_count += 1;
}

void setPixelColors(led_strip::PixelManager &manager,
                    Adafruit_NeoPixel &pixels) {
  for (uint16_t i = 0; i < pixels.numPixels(); i++) {
    pixels.setPixelColor(i, manager.pixel_units[i].hex());
  }
}

float getSrcValue(DataSource input_src) {
  float sensor_value = 0.0f;
  float t = (millis() / 1.0e3f);
  switch (input_src) {
    case DataSource::BeatSin05:
      // 0.5 Hz
      sensor_value = 0.5f * sinf(2.0f * M_PI * 0.5f * t) + 0.5f;
      break;
    case DataSource::BeatSin10:
      // 1.0 Hz
      sensor_value = 0.5 * sinf(2.0f * M_PI * 1.0f * t) + 0.5f;
      break;
    case DataSource::BeatSin20:
      // 2.0 Hz
      sensor_value = 0.5 * sinf(2.0f * M_PI * 2.0f * t) + 0.5f;
      break;
    case DataSource::BeatSaw05:
      // 0.5 Hz
      sensor_value = 0.5f * t - floorf(0.5f * t);
      break;
    case DataSource::BeatSaw10:
      // 1.0 Hz
      sensor_value = t - floorf(t);
      break;
    case DataSource::BeatSaw20:
      // 0.5 Hz
      sensor_value = 2.0f * t - floorf(2.0f * t);
      break;
    default:
      break;
  }
  return sensor_value;
}

void updatePixelColors(led_strip::PixelManager &manager, DataSource input_src) {
  // read sensor value
  float sensor_value = getSrcValue(input_src);
  // compute intensity
  manager.computeAndSetIntensity(sensor_value);
  manager.setColor();
}
#ifdef LSM6DS3_ENABLED
float getSrcValue(DataSource input_src, LSM6DS3 &imu) {
  float sensor_value = 0.0f;
  switch (input_src) {
    case DataSource::AccX:
      sensor_value =
          easing::remap(imu.readFloatAccelX(), -2.0f, 2.0f, -1.0f, 1.0f);
      break;
    case DataSource::AccY:
      sensor_value =
          easing::remap(imu.readFloatAccelY(), -2.0f, 2.0f, -1.0f, 1.0f);
      break;
    case DataSource::AccZ:
      sensor_value =
          easing::remap(imu.readFloatAccelZ(), -2.0f, 2.0f, -1.0f, 1.0f);
      break;
    case DataSource::GyroX:
      sensor_value =
          easing::remap(imu.readFloatGyroX(), -180.0f, 180.0f, -1.0f, 1.0f);
      break;
    case DataSource::GyroY:
      sensor_value =
          easing::remap(imu.readFloatGyroY(), -180.0f, 180.0f, -1.0f, 1.0f);
      break;
    case DataSource::GyroZ:
      sensor_value =
          easing::remap(imu.readFloatGyroZ(), -180.0f, 180.0f, -1.0f, 1.0f);
      break;
  }

  return sensor_value;
}
void updatePixelColors(led_strip::PixelManager &manager, LSM6DS3 &imu,
                       DataSource input_src) {
  // read sensor value
  float sensor_value = 0.0f;
  sensor_value = getSrcValue(input_src);
  if (sensor_value < 1e-6f) {
    sensor_value = getSrcValue(input_src, imu);
  }

  // compute intensity
  manager.computeAndSetIntensity(sensor_value);
  manager.setColor();
}
#endif

}  // namespace tasks

#endif
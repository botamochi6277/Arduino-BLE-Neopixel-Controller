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
        "Beat/Sine/0.5Hz", "Beat/Sine/1.0Hz", "Beat/Sine/2.0Hz",
        "Beat/Saw/0.5Hz",  "Beat/Saw/1.0Hz",  "Beat/Saw/2.0Hz",
        "Acc/X",           "Acc/Y",           "Acc/Z",
        "Acc/Mag",         "Gyro/X",          "Gyro/Y",
        "Gyro/Z",
    };
    return names[static_cast<uint8_t>(id)];
}

void reflectParams(ble::NeoPixelService &pixel_srv, Adafruit_NeoPixel &pixels,
                   bool is_on_written = true) {
    static unsigned int loop_count = 0;
    if (!is_on_written | pixel_srv.brightness_chr.written()) {
        pixels.setBrightness(pixel_srv.brightness_chr.value());
    }

    // colormap
    if (!is_on_written | pixel_srv.colormap_chr.written()) {
        auto cmap_id =
            static_cast<colormap::ColormapId>(pixel_srv.colormap_chr.value());
        if (cmap_id < colormap::ColormapId::LENGTH) {
            pixel_srv.colormap_name_chr.writeValue(
                colormap::colormap_name(cmap_id));
        }
    }

    // intensity function
    if (!is_on_written | pixel_srv.intensity_func_chr.written()) {
        auto func_id = static_cast<shape::IntensityFuncId>(
            pixel_srv.intensity_func_chr.value());
        if (func_id < shape::IntensityFuncId::LENGTH) {
            pixel_srv.intensity_name_chr.writeValue(
                shape::intensity_func_name(func_id));
        }
    }

    // data source
    if (!is_on_written | pixel_srv.input_chr.written()) {
        auto data_id =
            static_cast<data_source::DataSource>(pixel_srv.input_chr.value());
        if (data_id < data_source::DataSource::LENGTH) {
            pixel_srv.input_name_chr.writeValue(
                data_source::input_name(data_id));
        }
    }

    loop_count += 1;
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
            pixel_srv.input_name_chr.writeValue(input_name(
                static_cast<DataSource>(pixel_srv.input_chr.valueLE())));
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

void setPixelColors(color::PixelUnit cache[], Adafruit_NeoPixel &pixels) {
    for (uint16_t i = 0; i < pixels.numPixels(); i++) {
        pixels.setPixelColor(i, cache[i].hex());
    }
}

float getSrcValue(DataSource input_src) {
    float magnitude = 0.0f;
    float t = (millis() / 1.0e3f);
    switch (input_src) {
        case DataSource::BeatSin05:
            // 0.5 Hz
            magnitude = 0.5f * sinf(2.0f * M_PI * 0.5f * t) + 0.5f;
            break;
        case DataSource::BeatSin10:
            // 1.0 Hz
            magnitude = 0.5 * sinf(2.0f * M_PI * 1.0f * t) + 0.5f;
            break;
        case DataSource::BeatSin20:
            // 2.0 Hz
            magnitude = 0.5 * sinf(2.0f * M_PI * 2.0f * t) + 0.5f;
            break;
        case DataSource::BeatSaw05:
            // 0.5 Hz
            magnitude = 0.5f * t - floorf(0.5f * t);
            break;
        case DataSource::BeatSaw10:
            // 1.0 Hz
            magnitude = t - floorf(t);
            break;
        case DataSource::BeatSaw20:
            // 0.5 Hz
            magnitude = 2.0f * t - floorf(2.0f * t);
            break;
        default:
            break;
    }
    return magnitude;
}

void updatePixelColors(led_strip::PixelManager &manager, DataSource input_src) {
    // read sensor value
    float magnitude = getSrcValue(input_src);
    // compute intensity
    manager.computeAndSetIntensity(magnitude);
    manager.setColor();
}
#ifdef LSM6DS3_ENABLED
float getSrcValue(DataSource input_src, LSM6DS3 &imu) {
    float magnitude = 0.0f;
    switch (input_src) {
        case DataSource::AccX:
            magnitude =
                easing::remap(imu.readFloatAccelX(), -2.0f, 2.0f, -1.0f, 1.0f);
            break;
        case DataSource::AccY:
            magnitude =
                easing::remap(imu.readFloatAccelY(), -2.0f, 2.0f, -1.0f, 1.0f);
            break;
        case DataSource::AccZ:
            magnitude =
                easing::remap(imu.readFloatAccelZ(), -2.0f, 2.0f, -1.0f, 1.0f);
            break;
        case DataSource::GyroX:
            magnitude = easing::remap(imu.readFloatGyroX(), -180.0f, 180.0f,
                                      -1.0f, 1.0f);
            break;
        case DataSource::GyroY:
            magnitude = easing::remap(imu.readFloatGyroY(), -180.0f, 180.0f,
                                      -1.0f, 1.0f);
            break;
        case DataSource::GyroZ:
            magnitude = easing::remap(imu.readFloatGyroZ(), -180.0f, 180.0f,
                                      -1.0f, 1.0f);
            break;
    }

    return magnitude;
}
void updatePixelColors(led_strip::PixelManager &manager, LSM6DS3 &imu,
                       DataSource input_src) {
    // read sensor value
    float magnitude = 0.0f;
    magnitude = getSrcValue(input_src);
    if (magnitude < 1e-6f) {
        magnitude = getSrcValue(input_src, imu);
    }

    // compute intensity from magnitude
    manager.computeAndSetIntensity(magnitude);

    // set colors from colormap
    manager.setColor();
}
#endif

}  // namespace tasks

#endif
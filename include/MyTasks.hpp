#ifndef BOTALAB_TASKS_HPP
#define BOTALAB_TASKS_HPP

#include <Adafruit_NeoPixel.h>
#include <TaskManager.h>
#ifdef LSM6DS3_ENABLED
#include "LSM6DS3.h"  // IMU chip
#endif

#include "Colormap.hpp"
#include "DataSource.hpp"
#include "NeopixelService.hpp"
#include "PixelUnit.hpp"
#include "Shapes.hpp"

using data_source::DataSource;

namespace tasks {

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

void setPixelColors(color::PixelUnit cache[], Adafruit_NeoPixel &pixels) {
    for (uint16_t i = 0; i < pixels.numPixels(); i++) {
        pixels.setPixelColor(i, cache[i].hex());
    }
}

}  // namespace tasks

#endif
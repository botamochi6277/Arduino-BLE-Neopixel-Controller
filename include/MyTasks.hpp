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

using colormap::ColormapId;
using data_source::DataSource;
using shape::IntensityFuncId;

namespace tasks {

void reflectParams(ble::NeoPixelService &pixel_srv, Adafruit_NeoPixel &pixels,
                   bool is_on_written = true) {
    static unsigned int loop_count = 0;
    if (!is_on_written | pixel_srv.brightness_chr.written()) {
        pixels.setBrightness(pixel_srv.brightness_chr.value());
    }

    // colormap
    if (!is_on_written | pixel_srv.colormap_chr.written()) {
        auto cmap_id = static_cast<ColormapId>(pixel_srv.colormap_chr.value());
        if (cmap_id < ColormapId::LENGTH) {
            pixel_srv.colormap_name_chr.writeValue(
                colormap::colormap_name(cmap_id));
        }
    }

    // intensity function
    if (!is_on_written | pixel_srv.intensity_func_chr.written()) {
        auto func_id =
            static_cast<IntensityFuncId>(pixel_srv.intensity_func_chr.value());
        if (func_id < IntensityFuncId::LENGTH) {
            pixel_srv.intensity_name_chr.writeValue(
                shape::intensity_func_name(func_id));
        }
    }

    // data source
    if (!is_on_written | pixel_srv.input_chr.written()) {
        auto data_id = static_cast<DataSource>(pixel_srv.input_chr.value());
        if (data_id < DataSource::LENGTH) {
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

unsigned char shiftCyclicVal(unsigned char current_val, unsigned char max_val,
                             bool is_increase = true,
                             unsigned char min_val = 0U) {
    if (is_increase) {
        if (current_val == max_val) {
            return min_val;
        } else {
            return current_val + 1;
        }
    } else {
        // decrease value
        if (current_val == min_val) {
            return max_val;
        } else {
            return current_val - 1;
        }
    }
}

#ifdef LSM6DS3_ENABLED
enum class Knock : unsigned char {
    Undetected,
    NegativeX,
    PositiveX,
    NegativeY,
    PositiveY,
    NegativeZ,
    PositiveZ,
    LENGTH
};
Knock detectKnock(LSM6DS3 &imu, float threshold) {
    if (imu.readFloatAccelX() < -threshold) {
        return Knock::NegativeX;
    }
    if (threshold < imu.readFloatAccelX()) {
        return Knock::PositiveX;
    }

    if (imu.readFloatAccelY() < -threshold) {
        return Knock::NegativeY;
    }
    if (threshold < imu.readFloatAccelY()) {
        return Knock::PositiveY;
    }

    if (imu.readFloatAccelZ() < -threshold) {
        return Knock::NegativeZ;
    }
    if (threshold < imu.readFloatAccelZ()) {
        return Knock::PositiveZ;
    }

    return Knock::Undetected;
}

void shiftChrValue(ble::NeoPixelService &srv, Knock knock) {
    unsigned char tmp = 0;
    switch (knock) {
        case Knock::PositiveX:
            tmp = shiftCyclicVal(
                srv.input_chr.valueLE(),
                static_cast<unsigned char>(DataSource::LENGTH) - 1);
            srv.input_chr.writeValueBE(tmp);
            break;
        case Knock::NegativeX:
            tmp = shiftCyclicVal(
                srv.input_chr.valueLE(),
                static_cast<unsigned char>(DataSource::LENGTH) - 1, false);
            srv.input_chr.writeValueBE(tmp);
            break;

        case Knock::PositiveY:
            tmp = shiftCyclicVal(
                srv.intensity_func_chr.valueLE(),
                static_cast<unsigned char>(IntensityFuncId::LENGTH) - 1);
            srv.intensity_func_chr.writeValueBE(tmp);
            break;
        case Knock::NegativeY:
            tmp = shiftCyclicVal(
                srv.intensity_func_chr.valueLE(),
                static_cast<unsigned char>(IntensityFuncId::LENGTH) - 1, false);
            srv.intensity_func_chr.writeValueBE(tmp);
            break;

        case Knock::PositiveZ:
            tmp = shiftCyclicVal(
                srv.colormap_chr.valueLE(),
                static_cast<unsigned char>(ColormapId::LENGTH) - 1);
            srv.colormap_chr.writeValueBE(tmp);
            break;
        case Knock::NegativeZ:
            tmp = shiftCyclicVal(
                srv.colormap_chr.valueLE(),
                static_cast<unsigned char>(ColormapId::LENGTH) - 1, false);
            srv.colormap_chr.writeValueBE(tmp);
            break;

        default:
            break;
    }
}

#endif

}  // namespace tasks

#endif
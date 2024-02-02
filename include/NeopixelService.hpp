// Neopixel BLE Service
#ifndef BLE_NEOPIXEL_SRV_HPP
#define BLE_NEOPIXEL_SRV_HPP
#include <Arduino.h>
#include <ArduinoBLE.h>

#include "BLEFormat.hpp"
#include "BLEUnit.hpp"

namespace ble {

class NeoPixelService : public BLEService {
 private:
  const uint8_t msec_format_[7] = {
      BLE_GATT_CPF_FORMAT_UINT64,
      0b11111101,                             // exp, milli, -3
      (uint8_t)BLE_GATT_CPF_UNIT_SEC,         // 0x13
      (uint8_t)(BLE_GATT_CPF_UNIT_SEC >> 8),  // 0x27
      0x01,
      0x00,
      0x00};

  const uint8_t cmd_format_[7] = {BLE_GATT_CPF_FORMAT_UINT8,
                                  0b0,  // exp
                                  (uint8_t)BLE_GATT_CPF_UNIT_UNITLESS,
                                  (uint8_t)(BLE_GATT_CPF_UNIT_UNITLESS >> 8),
                                  0x01,
                                  0x00,
                                  0x00};

 public:
  // general
  BLEUnsignedLongCharacteristic timer_chr;
  BLEUnsignedCharCharacteristic imu_available_chr;

  // pixel basic
  BLEUnsignedCharCharacteristic num_pixels_chr;
  BLEUnsignedCharCharacteristic brightness_chr;

  // color
  // BLEUnsignedCharCharacteristic num_colors_chr;
  BLEUnsignedCharCharacteristic colormap_chr;
  BLEStringCharacteristic colormap_name_chr;

  // intensity
  BLEUnsignedCharCharacteristic intensity_func_chr;
  BLEStringCharacteristic intensity_name_chr;
  // intensity params
  BLEUnsignedCharCharacteristic wave_width_chr;
  BLEUnsignedCharCharacteristic wave_freq_chr;
  BLEUnsignedCharCharacteristic wave_speed_chr;

  // inputs
  BLEUnsignedCharCharacteristic input_chr;
  BLEStringCharacteristic input_name_chr;

  NeoPixelService(/* args */);
  ~NeoPixelService();
  // void init();
  void init(uint8_t brightness, uint8_t sensor_id, uint8_t intensity_id,
            uint8_t cmap);
};

NeoPixelService::NeoPixelService()
    : BLEService("19B10000-E8F2-537E-4F6C-D104768A1214"),
      timer_chr("19B10001-E8F2-537E-4F6C-D104768A1214", BLERead | BLENotify),
      imu_available_chr("19B10002-E8F2-537E-4F6C-D104768A1214", BLERead),
      num_pixels_chr("19B10011-E8F2-537E-4F6C-D104768A1214", BLERead),
      brightness_chr("19B10012-E8F2-537E-4F6C-D104768A1214",
                     BLERead | BLEWrite),
      colormap_chr("19B10021-E8F2-537E-4F6C-D104768A1214", BLERead | BLEWrite),
      colormap_name_chr("19B10022-E8F2-537E-4F6C-D104768A1214",
                        BLERead | BLEWrite, 12),
      intensity_func_chr("19B10031-E8F2-537E-4F6C-D104768A1214",
                         BLERead | BLEWrite),
      intensity_name_chr("19B10032-E8F2-537E-4F6C-D104768A1214",
                         BLERead | BLEWrite, 8),
      wave_width_chr("19B10033-E8F2-537E-4F6C-D104768A1214",
                     BLERead | BLEWrite),
      wave_freq_chr("19B10034-E8F2-537E-4F6C-D104768A1214", BLERead | BLEWrite),
      wave_speed_chr("19B10034-E8F2-537E-4F6C-D104768A1214",
                     BLERead | BLEWrite),
      input_chr("19B10041-E8F2-537E-4F6C-D104768A1214", BLERead | BLEWrite),
      input_name_chr("19B10042-E8F2-537E-4F6C-D104768A1214", BLERead | BLEWrite,
                     8) {
  // add characteristics to service
  this->addCharacteristic(this->timer_chr);
  this->addCharacteristic(this->imu_available_chr);

  this->addCharacteristic(this->num_pixels_chr);
  this->addCharacteristic(this->brightness_chr);

  this->addCharacteristic(this->colormap_chr);
  this->addCharacteristic(this->colormap_name_chr);

  this->addCharacteristic(this->intensity_func_chr);
  this->addCharacteristic(this->intensity_name_chr);

  this->addCharacteristic(this->wave_width_chr);
  this->addCharacteristic(this->wave_freq_chr);
  this->addCharacteristic(this->wave_speed_chr);

  this->addCharacteristic(this->input_chr);
  this->addCharacteristic(this->input_name_chr);

  // User Description
  // system property characteristic
  BLEDescriptor timer_descriptor("2901", "timer_ms");
  this->timer_chr.addDescriptor(timer_descriptor);
  BLEDescriptor imu_available_descriptor("2901", "imu available");
  this->imu_available_chr.addDescriptor(imu_available_descriptor);

  // pixel general
  BLEDescriptor num_pixels_descriptor("2901", "#pixels");
  this->num_pixels_chr.addDescriptor(num_pixels_descriptor);
  BLEDescriptor brightness_dsp("2901", "brightness");
  this->brightness_chr.addDescriptor(brightness_dsp);

  // colormap
  BLEDescriptor cmap_descriptor("2901", "colormap::id");
  this->colormap_chr.addDescriptor(cmap_descriptor);
  BLEDescriptor cmap_name_descriptor("2901", "colormap::name");
  this->colormap_name_chr.addDescriptor(cmap_name_descriptor);

  // intensity
  BLEDescriptor i_func_descriptor("2901", "intensity_func::id");
  this->intensity_func_chr.addDescriptor(i_func_descriptor);
  BLEDescriptor func_name_descriptor("2901", "intensity_func::name");
  this->intensity_name_chr.addDescriptor(func_name_descriptor);

  BLEDescriptor wave_width_descriptor("2901", "intensity_param::wave_width");
  this->wave_width_chr.addDescriptor(wave_width_descriptor);
  BLEDescriptor wave_freq_descriptor("2901", "intensity_param::wave_freq");
  this->wave_freq_chr.addDescriptor(wave_freq_descriptor);
  BLEDescriptor wave_speed_descriptor("2901", "intensity_param::wave_speed");
  this->wave_speed_chr.addDescriptor(wave_speed_descriptor);

  // inputs
  BLEDescriptor input_descriptor("2901", "input::id");
  this->input_chr.addDescriptor(input_descriptor);
  BLEDescriptor input_name_descriptor("2901", "input::name");
  this->input_name_chr.addDescriptor(input_name_descriptor);

  // Format Description
  BLEDescriptor millisec_descriptor("2904", this->msec_format_, 7);
  this->timer_chr.addDescriptor(millisec_descriptor);
  BLEDescriptor imu_unitless_descriptor("2904", this->cmd_format_, 7);
  this->imu_available_chr.addDescriptor(imu_unitless_descriptor);

  BLEDescriptor brightness_unitless_descriptor("2904", this->cmd_format_, 7);
  this->brightness_chr.addDescriptor(brightness_unitless_descriptor);

  BLEDescriptor num_pixels_unitless_descriptor("2904", this->cmd_format_, 7);
  this->num_pixels_chr.addDescriptor(num_pixels_unitless_descriptor);

  BLEDescriptor source_unitless_descriptor("2904", this->cmd_format_, 7);
  this->input_chr.addDescriptor(source_unitless_descriptor);
  BLEDescriptor colormap_unitless_descriptor("2904", this->cmd_format_, 7);
  this->colormap_chr.addDescriptor(colormap_unitless_descriptor);
  BLEDescriptor func_unitless_descriptor("2904", this->cmd_format_, 7);
  this->intensity_func_chr.addDescriptor(func_unitless_descriptor);
}

NeoPixelService::~NeoPixelService(){};

void NeoPixelService::init(uint8_t brightness, uint8_t sensor_id,
                           uint8_t intensity_id, uint8_t cmap) {
  this->brightness_chr.writeValue(brightness);
  this->num_pixels_chr.writeValue(NUM_PIXELS);
  this->input_chr.writeValue(sensor_id);
  this->intensity_func_chr.writeValue(intensity_id);
  this->colormap_chr.writeValue(cmap);

  this->wave_width_chr.setValue(128U);
  this->wave_freq_chr.setValue(128U);
  this->wave_speed_chr.setValue(172U);
}

}  // namespace ble

#endif
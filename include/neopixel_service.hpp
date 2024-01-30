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
  const uint8_t color_format_[7] = {BLE_GATT_CPF_FORMAT_UINT32,
                                    0b0,  // exp
                                    (uint8_t)BLE_GATT_CPF_UNIT_HSB,
                                    (uint8_t)(BLE_GATT_CPF_UNIT_HSB >> 8),
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

  // colors basic
  BLEUnsignedCharCharacteristic num_pixels_chr;
  BLEUnsignedCharCharacteristic brightness_chr;

  // color palette
  BLEUnsignedCharCharacteristic num_colors_chr;
  BLEUnsignedIntCharacteristic color01_chr;
  BLEUnsignedIntCharacteristic color02_chr;
  BLEUnsignedIntCharacteristic color03_chr;
  BLEUnsignedIntCharacteristic color04_chr;

  BLEUnsignedCharCharacteristic colormap_chr;       // gradation, cycle
  BLEUnsignedCharCharacteristic lighting_mode_chr;  // (acc,time)
  BLEUnsignedCharCharacteristic transition_chr;     // dissolve, slide, etc.

  NeoPixelService(/* args */);
  ~NeoPixelService();
};

NeoPixelService::NeoPixelService()
    : BLEService("19B10000-E8F2-537E-4F6C-D104768A1214"),
      timer_chr("19B10001-E8F2-537E-4F6C-D104768A1214", BLERead | BLENotify),
      imu_available_chr("19B10002-E8F2-537E-4F6C-D104768A1214", BLERead),
      num_pixels_chr("19B10011-E8F2-537E-4F6C-D104768A1214", BLERead),
      brightness_chr("19B10012-E8F2-537E-4F6C-D104768A1214",
                     BLERead | BLEWrite),
      num_colors_chr("19B10021-E8F2-537E-4F6C-D104768A1214",
                     BLERead | BLEWrite),
      color01_chr("19B10022-E8F2-537E-4F6C-D104768A1214", BLERead | BLEWrite),
      color02_chr("19B10023-E8F2-537E-4F6C-D104768A1214", BLERead | BLEWrite),
      color03_chr("19B10024-E8F2-537E-4F6C-D104768A1214", BLERead | BLEWrite),
      color04_chr("19B10025-E8F2-537E-4F6C-D104768A1214", BLERead | BLEWrite),
      colormap_chr("19B10026-E8F2-537E-4F6C-D104768A1214", BLERead | BLEWrite),
      transition_chr("19B10027-E8F2-537E-4F6C-D104768A1214",
                     BLERead | BLEWrite),
      lighting_mode_chr("19B10028-E8F2-537E-4F6C-D104768A1214",
                        BLERead | BLEWrite) {
  // add characteristics to service
  this->addCharacteristic(this->timer_chr);
  this->addCharacteristic(this->imu_available_chr);

  this->addCharacteristic(this->num_pixels_chr);
  this->addCharacteristic(this->brightness_chr);

  this->addCharacteristic(this->num_colors_chr);
  this->addCharacteristic(this->color01_chr);
  this->addCharacteristic(this->color02_chr);
  this->addCharacteristic(this->color03_chr);
  this->addCharacteristic(this->color04_chr);
  this->addCharacteristic(this->colormap_chr);
  this->addCharacteristic(this->transition_chr);
  this->addCharacteristic(this->lighting_mode_chr);

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

  // palette
  BLEDescriptor num_colors_descriptor("2901", "#used_colors");
  this->num_colors_chr.addDescriptor(num_colors_descriptor);

  BLEDescriptor color01_descriptor("2901", "color01 (HSB)");
  BLEDescriptor color02_descriptor("2901", "color02 (HSB)");
  BLEDescriptor color03_descriptor("2901", "color03 (HSB)");
  BLEDescriptor color04_descriptor("2901", "color04 (HSB)");
  this->color01_chr.addDescriptor(color01_descriptor);
  this->color02_chr.addDescriptor(color02_descriptor);
  this->color03_chr.addDescriptor(color03_descriptor);
  this->color04_chr.addDescriptor(color04_descriptor);

  BLEDescriptor blending_descriptor("2901", "color blending type");
  this->colormap_chr.addDescriptor(blending_descriptor);
  BLEDescriptor transition_descriptor("2901", "transition type");
  this->transition_chr.addDescriptor(transition_descriptor);
  BLEDescriptor fluctuation_descriptor("2901", "fluctuation");
  this->lighting_mode_chr.addDescriptor(fluctuation_descriptor);

  // Format Description
  BLEDescriptor millisec_descriptor("2904", this->msec_format_, 7);
  this->timer_chr.addDescriptor(millisec_descriptor);
  BLEDescriptor imu_unitless_descriptor("2904", this->cmd_format_, 7);
  this->imu_available_chr.addDescriptor(imu_unitless_descriptor);

  BLEDescriptor num_colors_unitless_descriptor("2904", this->cmd_format_, 7);
  this->num_colors_chr.addDescriptor(num_colors_unitless_descriptor);
  BLEDescriptor brightness_unitless_descriptor("2904", this->cmd_format_, 7);
  this->brightness_chr.addDescriptor(brightness_unitless_descriptor);

  BLEDescriptor num_pixels_unitless_descriptor("2904", this->cmd_format_, 7);
  this->num_pixels_chr.addDescriptor(num_pixels_unitless_descriptor);
  BLEDescriptor color_unitless_descriptor01("2904", this->color_format_, 7);
  this->color01_chr.addDescriptor(color_unitless_descriptor01);
  BLEDescriptor color_unitless_descriptor02("2904", this->color_format_, 7);
  this->color02_chr.addDescriptor(color_unitless_descriptor02);
  BLEDescriptor color_unitless_descriptor03("2904", this->color_format_, 7);
  this->color03_chr.addDescriptor(color_unitless_descriptor03);
  BLEDescriptor color_unitless_descriptor04("2904", this->color_format_, 7);
  this->color04_chr.addDescriptor(color_unitless_descriptor04);

  BLEDescriptor trans_unitless_descriptor("2904", this->cmd_format_, 7);
  this->transition_chr.addDescriptor(trans_unitless_descriptor);
  BLEDescriptor blending_unitless_descriptor("2904", this->cmd_format_, 7);
  this->colormap_chr.addDescriptor(blending_unitless_descriptor);
  BLEDescriptor fluctuation_unitless_descriptor("2904", this->cmd_format_, 7);
  this->lighting_mode_chr.addDescriptor(fluctuation_unitless_descriptor);
}

NeoPixelService::~NeoPixelService(){};

}  // namespace ble

#endif
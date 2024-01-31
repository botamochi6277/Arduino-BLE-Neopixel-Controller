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
  BLEUnsignedCharCharacteristic colormap_chr;
  BLEUnsignedCharCharacteristic source_chr;
  BLEUnsignedCharCharacteristic intensity_func_chr;

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
      num_colors_chr("19B10021-E8F2-537E-4F6C-D104768A1214",
                     BLERead | BLEWrite),
      colormap_chr("19B10026-E8F2-537E-4F6C-D104768A1214", BLERead | BLEWrite),
      source_chr("19B10027-E8F2-537E-4F6C-D104768A1214", BLERead | BLEWrite),
      intensity_func_chr("19B10028-E8F2-537E-4F6C-D104768A1214",
                         BLERead | BLEWrite) {
  // add characteristics to service
  this->addCharacteristic(this->timer_chr);
  this->addCharacteristic(this->imu_available_chr);

  this->addCharacteristic(this->num_pixels_chr);
  this->addCharacteristic(this->brightness_chr);

  this->addCharacteristic(this->num_colors_chr);
  this->addCharacteristic(this->colormap_chr);
  this->addCharacteristic(this->source_chr);
  this->addCharacteristic(this->intensity_func_chr);

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

  BLEDescriptor cmap_descriptor("2901", "colormap ID");
  this->colormap_chr.addDescriptor(cmap_descriptor);
  BLEDescriptor src_descriptor("2901", "source data ID");
  this->source_chr.addDescriptor(src_descriptor);
  BLEDescriptor i_func_descriptor("2901", "intensity func ID");
  this->intensity_func_chr.addDescriptor(i_func_descriptor);

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

  BLEDescriptor source_unitless_descriptor("2904", this->cmd_format_, 7);
  this->source_chr.addDescriptor(source_unitless_descriptor);
  BLEDescriptor colormap_unitless_descriptor("2904", this->cmd_format_, 7);
  this->colormap_chr.addDescriptor(colormap_unitless_descriptor);
  BLEDescriptor func_unitless_descriptor("2904", this->cmd_format_, 7);
  this->intensity_func_chr.addDescriptor(func_unitless_descriptor);
}

NeoPixelService::~NeoPixelService(){};

// void NeoPixelService::init() {
//   this->init(20U, uint8_t Timer,
//              uint8_t::TravelingWave,
//              colormap::ColormapId::Hsv);
// }
void NeoPixelService::init(uint8_t brightness, uint8_t sensor_id,
                           uint8_t intensity_id, uint8_t cmap) {
  this->brightness_chr.writeValue(brightness);
  this->num_pixels_chr.writeValue(NUM_PIXELS);
  this->source_chr.writeValue(sensor_id);
  this->intensity_func_chr.writeValue(intensity_id);
  this->colormap_chr.writeValue(cmap);
}

}  // namespace ble

#endif
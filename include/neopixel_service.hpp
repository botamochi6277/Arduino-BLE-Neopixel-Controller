// Neopixel BLE Service
#ifndef BLE_NEOPIXEL_SRV_HPP
#define BLE_NEOPIXEL_SRV_HPP
#include <Arduino.h>
#include <ArduinoBLE.h>

#include "BLEFormat.hpp"
#include "BLEUnit.hpp"

namespace ble {

class NeopixelService : public BLEService {
 private:
  BLEUnsignedLongCharacteristic timer_char_;
  BLEUnsignedCharCharacteristic num_pixels_;
  BLEUnsignedCharCharacteristic num_colors_;
  BLEUnsignedIntCharacteristic color01_;
  BLEUnsignedIntCharacteristic color02_;
  BLEUnsignedIntCharacteristic color03_;
  BLEUnsignedIntCharacteristic color04_;

  BLEUnsignedCharCharacteristic transition_;  // dissolve, slide, etc.

  const uint8_t msec_format_[7] = {
      BLE_GATT_CPF_FORMAT_UINT64,
      0b11111101,                             // exp, milli, -3
      (uint8_t)BLE_GATT_CPF_UNIT_SEC,         // 0x13
      (uint8_t)(BLE_GATT_CPF_UNIT_SEC >> 8),  // 0x27
      0x01,
      0x00,
      0x00};
  const uint8_t color_format_[7] = {BLE_GATT_CPF_FORMAT_UINT64,
                                    0b0,  // exp
                                    (uint8_t)BLE_GATT_CPF_UNIT_UNITLESS,
                                    (uint8_t)(BLE_GATT_CPF_UNIT_UNITLESS >> 8),
                                    0x01,
                                    0x00,
                                    0x00};

 public:
  NeopixelService(/* args */);
  ~NeopixelService();
};

NeopixelService::NeopixelService()
    : BLEService("19B10000-E8F2-537E-4F6C-D104768A1214"),
      timer_char_("19B10001-E8F2-537E-4F6C-D104768A1214", BLERead | BLENotify),
      num_pixels_("19B10002-E8F2-537E-4F6C-D104768A1214", BLERead | BLENotify),
      num_colors_("19B10003-E8F2-537E-4F6C-D104768A1214",
                  BLERead | BLEWrite | BLENotify),
      color01_("19B10004-E8F2-537E-4F6C-D104768A1214",
               BLERead | BLEWrite | BLENotify),
      color02_("19B10005-E8F2-537E-4F6C-D104768A1214",
               BLERead | BLEWrite | BLENotify),
      color03_("19B10006-E8F2-537E-4F6C-D104768A1214",
               BLERead | BLEWrite | BLENotify),
      color04_("19B10007-E8F2-537E-4F6C-D104768A1214",
               BLERead | BLEWrite | BLENotify),
      transition_("19B10008-E8F2-537E-4F6C-D104768A1214",
                  BLERead | BLEWrite | BLENotify) {
  // add characteristics to service
  this->addCharacteristic(this->timer_char_);
  this->addCharacteristic(this->num_pixels_);
  this->addCharacteristic(this->num_colors_);
  this->addCharacteristic(this->color01_);
  this->addCharacteristic(this->color02_);
  this->addCharacteristic(this->color03_);
  this->addCharacteristic(this->color04_);
  this->addCharacteristic(this->transition_);

  // User Description
  BLEDescriptor timer_descriptor("2901", "timer_ms");
  this->timer_char_.addDescriptor(timer_descriptor);

  BLEDescriptor num_pixels_descriptor("2901", "#pixels");
  BLEDescriptor num_colors_descriptor("2901", "#used_colors");
  this->num_pixels_.addDescriptor(num_pixels_descriptor);
  this->num_colors_.addDescriptor(num_colors_descriptor);

  BLEDescriptor color01_descriptor("2901", "color01");
  BLEDescriptor color02_descriptor("2901", "color02");
  BLEDescriptor color03_descriptor("2901", "color03");
  BLEDescriptor color04_descriptor("2901", "color04");
  this->color01_.addDescriptor(color01_descriptor);
  this->color02_.addDescriptor(color02_descriptor);
  this->color03_.addDescriptor(color03_descriptor);
  this->color04_.addDescriptor(color04_descriptor);

  BLEDescriptor transition_descriptor("2901", "transition");
  this->transition_.addDescriptor(transition_descriptor);
}

NeopixelService::~NeopixelService(){};

}  // namespace ble

#endif
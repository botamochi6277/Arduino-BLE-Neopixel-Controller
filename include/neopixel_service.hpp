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
  BLEUnsignedLongCharacteristic timer_chr;
  BLEUnsignedCharCharacteristic num_pixels_chr;
  BLEUnsignedCharCharacteristic num_colors_chr;
  BLEUnsignedIntCharacteristic color01_chr;
  BLEUnsignedIntCharacteristic color02_chr;
  BLEUnsignedIntCharacteristic color03_chr;
  BLEUnsignedIntCharacteristic color04_chr;

  BLEUnsignedCharCharacteristic transition_chr;  // dissolve, slide, etc.
  BLEUnsignedCharCharacteristic noise_chr;       // (acc,time)
  BLEUnsignedCharCharacteristic blending_chr;    // gradation, cycle

  NeopixelService(/* args */);
  ~NeopixelService();
};

NeopixelService::NeopixelService()
    : BLEService("19B10000-E8F2-537E-4F6C-D104768A1214"),
      timer_chr("19B10001-E8F2-537E-4F6C-D104768A1214", BLERead | BLENotify),
      num_pixels_chr("19B10002-E8F2-537E-4F6C-D104768A1214",
                     BLERead | BLENotify),
      num_colors_chr("19B10003-E8F2-537E-4F6C-D104768A1214",
                     BLERead | BLEWrite | BLENotify),
      color01_chr("19B10004-E8F2-537E-4F6C-D104768A1214",
                  BLERead | BLEWrite | BLENotify),
      color02_chr("19B10005-E8F2-537E-4F6C-D104768A1214",
                  BLERead | BLEWrite | BLENotify),
      color03_chr("19B10006-E8F2-537E-4F6C-D104768A1214",
                  BLERead | BLEWrite | BLENotify),
      color04_chr("19B10007-E8F2-537E-4F6C-D104768A1214",
                  BLERead | BLEWrite | BLENotify),
      blending_chr("19B10008-E8F2-537E-4F6C-D104768A1214",
                   BLERead | BLEWrite | BLENotify),
      transition_chr("19B10009-E8F2-537E-4F6C-D104768A1214",
                     BLERead | BLEWrite | BLENotify),
      noise_chr("19B10010-E8F2-537E-4F6C-D104768A1214",
                BLERead | BLEWrite | BLENotify) {
  // add characteristics to service
  this->addCharacteristic(this->timer_chr);
  this->addCharacteristic(this->num_pixels_chr);
  this->addCharacteristic(this->num_colors_chr);
  this->addCharacteristic(this->color01_chr);
  this->addCharacteristic(this->color02_chr);
  this->addCharacteristic(this->color03_chr);
  this->addCharacteristic(this->color04_chr);
  this->addCharacteristic(this->blending_chr);
  this->addCharacteristic(this->transition_chr);
  this->addCharacteristic(this->noise_chr);

  // User Description
  BLEDescriptor timer_descriptor("2901", "timer_ms");
  this->timer_chr.addDescriptor(timer_descriptor);

  BLEDescriptor num_pixels_descriptor("2901", "#pixels");
  BLEDescriptor num_colors_descriptor("2901", "#used_colors");
  this->num_pixels_chr.addDescriptor(num_pixels_descriptor);
  this->num_colors_chr.addDescriptor(num_colors_descriptor);

  BLEDescriptor color01_descriptor("2901", "color01");
  BLEDescriptor color02_descriptor("2901", "color02");
  BLEDescriptor color03_descriptor("2901", "color03");
  BLEDescriptor color04_descriptor("2901", "color04");
  this->color01_chr.addDescriptor(color01_descriptor);
  this->color02_chr.addDescriptor(color02_descriptor);
  this->color03_chr.addDescriptor(color03_descriptor);
  this->color04_chr.addDescriptor(color04_descriptor);

  BLEDescriptor blending_descriptor("2901", "color blending type");
  this->blending_chr.addDescriptor(blending_descriptor);
  BLEDescriptor transition_descriptor("2901", "transition type");
  this->transition_chr.addDescriptor(transition_descriptor);
  BLEDescriptor noise_descriptor("2901", "noise");
  this->noise_chr.addDescriptor(noise_descriptor);
}

NeopixelService::~NeopixelService(){};

}  // namespace ble

#endif
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

    const uint8_t freq_format_[7] = {BLE_GATT_CPF_FORMAT_FLOAT32,
                                     0,
                                     (uint8_t)BLE_GATT_CPF_UNIT_HERTZ,
                                     (uint8_t)(BLE_GATT_CPF_UNIT_HERTZ >> 8),
                                     0x01,
                                     0x00,
                                     0x00};

    const uint8_t unitless_format_[7] = {
        BLE_GATT_CPF_FORMAT_FLOAT32,
        0,
        (uint8_t)BLE_GATT_CPF_UNIT_UNITLESS,
        (uint8_t)(BLE_GATT_CPF_UNIT_UNITLESS >> 8),
        0x01,
        0x00,
        0x00};
    const uint8_t string_format_[7] = {
        BLE_GATT_CPF_FORMAT_UTF8S,
        0,
        (uint8_t)BLE_GATT_CPF_UNIT_UNITLESS,
        (uint8_t)(BLE_GATT_CPF_UNIT_UNITLESS >> 8),
        0x01,
        0x00,
        0x00};

   public:
    // general
    BLEUnsignedLongCharacteristic timer_chr;
    BLEUnsignedCharCharacteristic imu_available_chr;
    BLEUnsignedCharCharacteristic knock_activate_chr;

    // pixel basic
    BLEUnsignedCharCharacteristic num_pixels_chr;
    BLEUnsignedCharCharacteristic brightness_chr;

    // inputs
    BLEUnsignedCharCharacteristic input_chr;
    BLEStringCharacteristic input_name_chr;
    BLEFloatCharacteristic magnitude_chr;

    // intensity
    BLEUnsignedCharCharacteristic intensity_func_chr;
    BLEStringCharacteristic intensity_name_chr;

    // intensity params
    BLEFloatCharacteristic wave_width_chr;
    // BLEFloatCharacteristic wave_freq_chr;
    BLEFloatCharacteristic wave_speed_chr;

    // colormap
    BLEUnsignedCharCharacteristic colormap_chr;
    BLEStringCharacteristic colormap_name_chr;

    NeoPixelService(/* args */);
    ~NeoPixelService();
    // void init();
    void init(uint8_t brightness, uint8_t num_pixels, uint8_t sensor_id,
              uint8_t intensity_id, uint8_t cmap);
};

NeoPixelService::NeoPixelService()
    : BLEService("19B10000-E8F2-537E-4F6C-D104768A1214"),
      // general
      timer_chr("19B10001-E8F2-537E-4F6C-D104768A1214", BLERead | BLENotify),
      imu_available_chr("19B10002-E8F2-537E-4F6C-D104768A1214", BLERead),
      knock_activate_chr("19B10003-E8F2-537E-4F6C-D104768A1214",
                         BLERead | BLEWrite),
      // pixel basic
      num_pixels_chr("19B10011-E8F2-537E-4F6C-D104768A1214", BLERead),
      brightness_chr("19B10012-E8F2-537E-4F6C-D104768A1214",
                     BLERead | BLEWrite),
      // input
      input_chr("19B10021-E8F2-537E-4F6C-D104768A1214", BLERead | BLEWrite),
      input_name_chr("19B10022-E8F2-537E-4F6C-D104768A1214",
                     BLERead | BLENotify, 24),
      magnitude_chr("19B10023-E8F2-537E-4F6C-D104768A1214",
                    BLERead | BLENotify),
      //  intensity
      intensity_func_chr("19B10031-E8F2-537E-4F6C-D104768A1214",
                         BLERead | BLEWrite),
      intensity_name_chr("19B10032-E8F2-537E-4F6C-D104768A1214",
                         BLERead | BLENotify, 24),
      wave_width_chr("19B10033-E8F2-537E-4F6C-D104768A1214",
                     BLERead | BLEWrite),
      // wave_freq_chr("19B10034-E8F2-537E-4F6C-D104768A1214", BLERead
      // |BLEWrite),
      wave_speed_chr("19B10034-E8F2-537E-4F6C-D104768A1214",
                     BLERead | BLEWrite),
      //  colormap
      colormap_chr("19B10041-E8F2-537E-4F6C-D104768A1214", BLERead | BLEWrite),
      colormap_name_chr("19B10042-E8F2-537E-4F6C-D104768A1214",
                        BLERead | BLENotify, 24)

{
    // add characteristics to service
    // general
    this->addCharacteristic(this->timer_chr);
    this->addCharacteristic(this->imu_available_chr);
    this->addCharacteristic(this->knock_activate_chr);
    // pixel
    this->addCharacteristic(this->num_pixels_chr);
    this->addCharacteristic(this->brightness_chr);
    // input
    this->addCharacteristic(this->input_chr);
    this->addCharacteristic(this->input_name_chr);
    this->addCharacteristic(this->magnitude_chr);

    // intensity
    this->addCharacteristic(this->intensity_func_chr);
    this->addCharacteristic(this->intensity_name_chr);

    this->addCharacteristic(this->wave_width_chr);
    // this->addCharacteristic(this->wave_freq_chr);
    this->addCharacteristic(this->wave_speed_chr);
    // colormap
    this->addCharacteristic(this->colormap_chr);
    this->addCharacteristic(this->colormap_name_chr);

    // User Description
    // system property characteristic
    BLEDescriptor timer_descriptor("2901", "timer_ms");
    this->timer_chr.addDescriptor(timer_descriptor);
    BLEDescriptor imu_available_descriptor("2901", "imu available");
    this->imu_available_chr.addDescriptor(imu_available_descriptor);
    BLEDescriptor knock_act_descriptor("2901", "knock is_activate");
    this->knock_activate_chr.addDescriptor(knock_act_descriptor);

    // pixel general
    BLEDescriptor num_pixels_descriptor("2901", "#pixels");
    this->num_pixels_chr.addDescriptor(num_pixels_descriptor);
    BLEDescriptor brightness_dsp("2901", "brightness");
    this->brightness_chr.addDescriptor(brightness_dsp);
    // inputs
    BLEDescriptor input_descriptor("2901", "input::id");
    this->input_chr.addDescriptor(input_descriptor);
    BLEDescriptor input_name_descriptor("2901", "input::name");
    this->input_name_chr.addDescriptor(input_name_descriptor);
    BLEDescriptor magnitude_descriptor("2901", "input::magnitude");
    this->magnitude_chr.addDescriptor(magnitude_descriptor);

    // intensity
    BLEDescriptor i_func_descriptor("2901", "intensity::id");
    this->intensity_func_chr.addDescriptor(i_func_descriptor);
    BLEDescriptor func_name_descriptor("2901", "intensity::name");
    this->intensity_name_chr.addDescriptor(func_name_descriptor);

    BLEDescriptor wave_width_descriptor("2901", "intensity::wave_width");
    this->wave_width_chr.addDescriptor(wave_width_descriptor);
    // BLEDescriptor wave_freq_descriptor("2901", "intensity::wave_freq");
    // this->wave_freq_chr.addDescriptor(wave_freq_descriptor);
    BLEDescriptor wave_speed_descriptor("2901", "intensity::wave_speed");
    this->wave_speed_chr.addDescriptor(wave_speed_descriptor);

    // colormap
    BLEDescriptor cmap_descriptor("2901", "colormap::id");
    this->colormap_chr.addDescriptor(cmap_descriptor);
    BLEDescriptor cmap_name_descriptor("2901", "colormap::name");
    this->colormap_name_chr.addDescriptor(cmap_name_descriptor);

    // Format Description
    BLEDescriptor millisec_descriptor("2904", this->msec_format_, 7);
    this->timer_chr.addDescriptor(millisec_descriptor);
    BLEDescriptor imu_unitless_descriptor("2904", this->cmd_format_, 7);
    this->imu_available_chr.addDescriptor(imu_unitless_descriptor);
    // pixels
    BLEDescriptor num_pixels_unitless_descriptor("2904", this->cmd_format_, 7);
    this->num_pixels_chr.addDescriptor(num_pixels_unitless_descriptor);

    BLEDescriptor brightness_unitless_descriptor("2904", this->cmd_format_, 7);
    this->brightness_chr.addDescriptor(brightness_unitless_descriptor);

    // ## assign format descriptor
    // ### inputs
    BLEDescriptor inputs_enum_descriptor("2904", this->cmd_format_, 7);
    this->input_chr.addDescriptor(inputs_enum_descriptor);
    BLEDescriptor str_input_format("2904", this->string_format_, 7);
    this->input_name_chr.addDescriptor(str_input_format);
    BLEDescriptor magnitude_input_format("2904", this->unitless_format_, 7);
    this->magnitude_chr.addDescriptor(magnitude_input_format);

    // ### intensity
    BLEDescriptor func_enum_descriptor("2904", this->cmd_format_, 7);
    this->intensity_func_chr.addDescriptor(func_enum_descriptor);
    BLEDescriptor str_func_format("2904", this->string_format_, 7);
    this->intensity_name_chr.addDescriptor(str_func_format);

    BLEDescriptor wave_w_f_descriptor("2904", this->unitless_format_, 7);
    this->wave_width_chr.addDescriptor(wave_w_f_descriptor);

    BLEDescriptor hz_descriptor("2904", this->freq_format_, 7);
    // this->wave_freq_chr.addDescriptor(hz_descriptor);

    BLEDescriptor sp_descriptor("2904", this->unitless_format_, 7);
    this->wave_speed_chr.addDescriptor(sp_descriptor);

    // ### colormap
    BLEDescriptor colormap_enum_descriptor("2904", this->cmd_format_, 7);
    this->colormap_chr.addDescriptor(colormap_enum_descriptor);
    BLEDescriptor str_colormap_format("2904", this->string_format_, 7);
    this->colormap_name_chr.addDescriptor(str_colormap_format);
}

NeoPixelService::~NeoPixelService() {};

void NeoPixelService::init(uint8_t brightness, uint8_t num_pixels,
                           uint8_t sensor_id, uint8_t intensity_id,
                           uint8_t cmap) {
    this->brightness_chr.writeValue(brightness);
    this->num_pixels_chr.writeValue(num_pixels);
    this->knock_activate_chr.writeValue(false);

    this->input_chr.writeValue(sensor_id);
    this->intensity_func_chr.writeValue(intensity_id);
    this->colormap_chr.writeValue(cmap);

    this->colormap_name_chr.writeValue(
        colormap::colormap_name(static_cast<colormap::ColormapId>(cmap)));

    this->wave_width_chr.writeValueLE(0.2f);
    // this->wave_freq_chr.writeValueLE(0.5f);
    this->wave_speed_chr.writeValueLE(0.2f);
}

}  // namespace ble

#endif
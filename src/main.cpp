// base code
// https://github.com/adafruit/Adafruit_NeoPixel/blob/master/examples/StrandtestArduinoBLE/StrandtestArduinoBLE.ino

// #include "FastLED.h" // FastLED unsupports nrf52 series
//  https://github.com/adafruit/Adafruit_NeoPixel
#include <Adafruit_NeoPixel.h>
#include <Arduino.h>
#include <ArduinoBLE.h>
#ifdef SEEED_XIAO_NRF52840_SENSE
#include "LSM6DS3.h"  // IMU chip
#endif
#include "Wire.h"
#include "color_utils.hpp"
#include "neopixel_service.hpp"
#include "preset.hpp"
#include "strip.hpp"

// NeoPixel variables
#ifdef ARDUINO_M5Stack_ATOM
#define PIXEL_PIN 32  // MOSI
#else
#define PIXEL_PIN 7  // MOSI
#endif
#define NUM_PIXELS 45
#define DELAY_MS 500

#define DELAY_LED 100

char message[128];  // buffer for sprintf

ble::NeopixelService pixel_srv;

unsigned long milli_sec;
float clock_sec;
float transition_start_sec;
float transition_duration = 3.0f;
float transition_progress = 0.0f;
unsigned int loop_count = 0;

bool transition_completed = false;  // flag for color transition
bool backward;

uint32_t current_colors[NUM_PIXELS];

uint32_t start_colors[NUM_PIXELS];
uint32_t transited_colors[NUM_PIXELS];
uint32_t cache_colors[NUM_PIXELS];

uint32_t palette[4];
float transition_weights[NUM_PIXELS];

// FLUCTUATION_TIME
uint32_t fluctuation_colors[NUM_PIXELS];

uint8_t red_buff;
uint8_t green_buff;
uint8_t blue_buff;
uint32_t color_buff;

Adafruit_NeoPixel pixels(NUM_PIXELS, PIXEL_PIN, NEO_GRB + NEO_KHZ800);

#ifdef SEEED_XIAO_NRF52840_SENSE
// Create a instance of class LSM6DS3
LSM6DS3 my_imu(I2C_MODE, 0x6A);  // I2C device address 0x6A
#endif

void setup() {
  // build-in leds
#ifdef SEEED_XIAO_NRF52840_SENSE
  pinMode(LEDR, OUTPUT);
  pinMode(LEDG, OUTPUT);
  pinMode(LEDB, OUTPUT);
  // turn off the all build-in leds
  digitalWrite(LEDR, HIGH);
  digitalWrite(LEDG, HIGH);
  digitalWrite(LEDB, HIGH);
#endif
  pinMode(PIXEL_PIN, OUTPUT);

  Serial.begin(115200);

  for (size_t i = 0; i < 10; i++) {
    if (Serial) {
      break;
    }
    delay(100);
  }

  Serial.println("NeoPixel BLE waking");

  // custom services and characteristics can be added as well
  // begin initialization
  if (!BLE.begin()) {
    Serial.println("starting BLE failed!");
    while (1) {
#ifdef SEEED_XIAO_NRF52840_SENSE
      digitalWrite(LEDR, !digitalRead(LEDR));
#endif
      delay(100);
    }
  }

  Serial.print("Peripheral address: ");
  String ble_address = BLE.address();
  Serial.println(ble_address);
  String local_name = "NeoPixels_" + ble_address;
  // set advertised local name and service UUID:
  BLE.setDeviceName(local_name.c_str());
  BLE.setLocalName("NeoPixels");
  BLE.setAdvertisedService(pixel_srv);

  // add service
  BLE.addService(pixel_srv);

  // start advertising
  BLE.advertise();

#ifdef SEEED_XIAO_NRF52840_SENSE
  if (my_imu.begin() != 0) {
    Serial.println("IMU error");
    pixel_srv.imu_available_chr.writeValue(0);
  } else {
    pixel_srv.imu_available_chr.writeValue(1);
  }
#endif

  // set the initial value for the characteristic:
  pixel_srv.brightness_chr.writeValue(20);
  pixel_srv.num_pixels_chr.writeValue(NUM_PIXELS);
  pixel_srv.num_colors_chr.writeValue(2U);
  pixel_srv.transition_chr.writeValue(TRANSITION_DISSOLVE);
  pixel_srv.color01_chr.writeValue(color::hsbToHsbhex(0x00, 0xff, 0xff));
  pixel_srv.color02_chr.writeValue(color::hsbToHsbhex(0xffff, 255U, 255U));
  pixel_srv.color04_chr.writeValue(color::hsbToHsbhex(0xffff, 255U, 255U));

  digitalWrite(PIXEL_PIN, LOW);
  pixels.begin();  // INITIALIZE NeoPixel strip object (REQUIRED)
  pixels.setBrightness(20);
  pixels.show();  // Turn OFF all pixels

  // init color buffer
  for (size_t i = 0; i < NUM_PIXELS; i++) {
    current_colors[i] = color::hsbToHsbhex(0xff00, 0xff, 0x00);
  }
  palette[0] = pixel_srv.color01_chr.value();
  palette[1] = pixel_srv.color02_chr.value();
  palette[2] = pixel_srv.color03_chr.value();
  palette[3] = pixel_srv.color04_chr.value();

  // temporal
  for (size_t i = 0; i < NUM_PIXELS; i++) {
    fluctuation_colors[i] = palette[3];
  }

  loop_count = 0;
}

void loop() {
  milli_sec = millis();
  clock_sec = milli_sec * 1.0e-3f;
  pixel_srv.timer_chr.writeValue(milli_sec);

#ifdef SEEED_XIAO_NRF52840_SENSE
  digitalWrite(LEDG, !digitalRead(LEDG));  // heartbeats
#endif
  BLE.poll();
  if (pixel_srv.brightness_chr.written()) {
    pixels.setBrightness(pixel_srv.brightness_chr.value());
  }
  // color parameter was changed, updating static colors
  if (pixel_srv.color01_chr.written() || pixel_srv.color02_chr.written() ||
      pixel_srv.color03_chr.written() || pixel_srv.color04_chr.written() ||
      pixel_srv.blending_chr.written() || pixel_srv.num_colors_chr.written() ||
      loop_count == 0) {
// re-blend static color
#ifdef SEEED_XIAO_NRF52840_SENSE
    digitalWrite(LEDB, !digitalRead(LEDB));  // response for ble value changing
#endif

    Serial.print("BLE params were updated, transition ");
    Serial.println(pixel_srv.transition_chr.value());

    transition_start_sec = clock_sec;
    // assign palette
    palette[0] = pixel_srv.color01_chr.value();
    palette[1] = pixel_srv.color02_chr.value();
    palette[2] = pixel_srv.color03_chr.value();
    palette[3] = pixel_srv.color04_chr.value();

    led_strip::blend(transited_colors, palette, NUM_PIXELS,
                     pixel_srv.num_colors_chr.value(),
                     pixel_srv.blending_chr.value());
    sprintf(message, "transition target color: $%08x--$%08x",
            transited_colors[0], transited_colors[NUM_PIXELS - 1]);
    Serial.println(message);

    transition_completed = false;

    for (size_t i = 0; i < NUM_PIXELS; i++) {
      fluctuation_colors[i] = palette[3];
    }

    for (size_t i = 0; i < NUM_PIXELS; i++) {
      start_colors[i] = current_colors[i];
    }
  }

  transition_progress =
      (clock_sec - transition_start_sec) / transition_duration;

  if (transition_completed == false) {
    switch (pixel_srv.transition_chr.value()) {
      case TRANSITION_DISSOLVE:
        transition_completed = led_strip::dissolveEasing(
            current_colors, start_colors, transited_colors, transition_weights,
            NUM_PIXELS, transition_progress);
        break;
      case TRANSITION_WIPE:
        transition_completed = led_strip::wipeEasing(
            current_colors, start_colors, transited_colors, transition_weights,
            NUM_PIXELS, transition_progress, false);
        break;
      case TRANSITION_SLIDE:
        transition_completed = led_strip::slideEasing(
            current_colors, start_colors, transited_colors, transition_weights,
            NUM_PIXELS, transition_progress, false);
        break;
      default:
        break;
    }
    if (transition_completed) {
      sprintf(message, "transition was over: %0.2f", transition_progress);
      Serial.println(message);
      sprintf(message, "final color: $%08x--$%08x", current_colors[0],
              current_colors[NUM_PIXELS - 1]);
      Serial.println(message);

      for (size_t i = 0; i < NUM_PIXELS; i++) {
        start_colors[i] = current_colors[i];
      }
      // led_strip::blend(fluctuation_colors, palette, NUM_PIXELS,
      //                  pixel_srv.num_colors_chr.value(),
      //                  pixel_srv.blending_chr.value());
    }

    // for (uint16_t i = 0; i < pixels.numPixels(); i++) {
    //   pixels.setPixelColor(i, color::hsbhexTohex(current_colors[i]));
    // }
    // pixels.show();
  }

  if (transition_completed) {
    // transition was completed
    // add fluctuation
    float p;  // cache, 0--1.0
    uint8_t color_idx;
    switch (pixel_srv.fluctuation_chr.value()) {
      case FLUCTUATION_TIME:
        p = 0.5f + 0.5f * std::sin(2.0f * M_PI * 0.1f * clock_sec);
        led_strip::dissolveEasing(current_colors, transited_colors,
                                  fluctuation_colors, transition_weights,
                                  NUM_PIXELS, p);
        break;
      case FLUCTUATION_PERIODIC:
#ifdef SEEED_XIAO_NRF52840_SENSE
        // hsb
        color_idx =
            easing::argmax3(my_imu.readFloatAccelX(), my_imu.readFloatAccelY(),
                            my_imu.readFloatAccelZ());
        if (color_idx == 0) {
          // x dominant
          color_buff = color::hsbToHsbhex(0x00, 0xff, 0xff);
          p = easing::remap(abs(my_imu.readFloatAccelX()), 0.0f, 1.5f, 0.0f,
                            1.0f, true);
        }
        if (color_idx == 1) {
          // y dominant
          color_buff = color::hsbToHsbhex(64U, 0xff, 0xff);
          p = easing::remap(abs(my_imu.readFloatAccelY()), 0.0f, 1.5f, 0.0f,
                            1.0f, true);
        }

        if (color_idx == 2) {
          // z dominant
          color_buff = color::hsbToHsbhex(128U, 0xff, 0xff);
          p = easing::remap(abs(my_imu.readFloatAccelZ()), 0.0f, 1.5f, 0.0f,
                            1.0f, true);
        }
        led_strip::fill(fluctuation_colors, NUM_PIXELS, color_buff);
        led_strip::dissolveEasing(current_colors, transited_colors,
                                  fluctuation_colors, transition_weights,
                                  NUM_PIXELS, p);
#endif
        break;
      default:
        // 0 is off
        break;
    }
  }

  for (uint16_t i = 0; i < pixels.numPixels(); i++) {
    pixels.setPixelColor(i, color::hsbhexTohex(current_colors[i]));
  }
  pixels.show();
  delay(DELAY_LED);
  loop_count++;
}

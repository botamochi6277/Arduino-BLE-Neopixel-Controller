// base code
// https://github.com/adafruit/Adafruit_NeoPixel/blob/master/examples/StrandtestArduinoBLE/StrandtestArduinoBLE.ino

// #include "FastLED.h" // FastLED unsupports nrf52 series
//  https://github.com/adafruit/Adafruit_NeoPixel

#ifdef SEEED_XIAO_NRF52840_SENSE
#define LSM6DS3_ENABLED
#endif

#include <Adafruit_NeoPixel.h>
#include <Arduino.h>
#include <ArduinoBLE.h>
#ifdef LSM6DS3_ENABLED
#include "LSM6DS3.h"  // IMU chip
#endif
#include <TaskManager.h>

#include "Wire.h"
#include "color_utils.hpp"
#include "neopixel_service.hpp"
#include "preset.hpp"
#include "strip.hpp"
#include "tasks.hpp"
#include "utils.hpp"

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

ble::NeoPixelService pixel_srv;

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
led_strip::PixelManager color_manager;

#ifdef LSM6DS3_ENABLED
// Create a instance of class LSM6DS3
LSM6DS3 my_imu(I2C_MODE, 0x6A);  // I2C device address 0x6A
#endif

void setup() {
#ifdef LSM6DS3_ENABLED
  init_leds(LEDR, LEDG, LEDB);
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

#ifdef LSM6DS3_ENABLED
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
    color_manager.setCurrentColor(i, color::hsbToHsbhex(0xff00, 0xff, 0x00));
    // current_colors[i] = color::hsbToHsbhex(0xff00, 0xff, 0x00);
  }
  color_manager.setPaletteColor(0, pixel_srv.color01_chr.value());
  color_manager.setPaletteColor(1, pixel_srv.color02_chr.value());
  color_manager.setPaletteColor(2, pixel_srv.color03_chr.value());
  color_manager.setPaletteColor(3, pixel_srv.color04_chr.value());

  // temporal
  for (size_t i = 0; i < NUM_PIXELS; i++) {
    color_manager.setFluctuationColor(i, color_manager.getPaletteColor(3));
  }

  loop_count = 0;

  Tasks.add("BLE_polling", [] { BLE.poll(); })->startFps(10);

#ifdef SEEED_XIAO_NRF52840_SENSE
  Tasks.add("Heat_beats", [] { digitalWrite(LEDG, !digitalRead(LEDG)); })
      ->startFps(1.0);
#endif
  Tasks
      .add("UpdatePixelColors",
           [] {
             tasks::setRainbow(color_manager, pixels, clock_sec, 10.0f);
             tasks::setPixelColors(color_manager, pixels);
             pixels.show();
           })
      ->startFps(24.0);
}

void loop() {
  milli_sec = millis();
  clock_sec = milli_sec * 1.0e-3f;
  pixel_srv.timer_chr.writeValue(milli_sec);
  Tasks.update();  // automatically execute tasks
  delay(1);
  loop_count++;
}

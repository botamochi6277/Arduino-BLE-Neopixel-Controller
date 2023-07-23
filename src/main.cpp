// base code
// https://github.com/adafruit/Adafruit_NeoPixel/blob/master/examples/StrandtestArduinoBLE/StrandtestArduinoBLE.ino

// #include "FastLED.h" // FastLED unsupports nrf52 series
//  https://github.com/adafruit/Adafruit_NeoPixel
#include <Adafruit_NeoPixel.h>
#include <Arduino.h>
#include <ArduinoBLE.h>

#include "LSM6DS3.h"  // IMU chip
#include "Wire.h"
#include "color_utils.hpp"
#include "neopixel_service.hpp"
#include "preset.hpp"
#include "strip.hpp"
// Neopixel variables
#define PIXEL_PIN 7  // MOSI
#define NUM_PIXELS 45
#define DELAY_MS 500

#define DELAY_LED 100

char message[128];  // buffer for sprintf

ble::NeopixelService pixel_srv;

unsigned long milli_sec;
float clock_sec;
float transition_start_sec;
float transition_duration = 3.0f;

bool transition_completed = false;  // flag for color transition
bool backward;

uint32_t current_colors[NUM_PIXELS];
uint32_t static_colors[NUM_PIXELS];

uint32_t start_colors[NUM_PIXELS];
uint32_t goal_colors[NUM_PIXELS];
uint32_t palette[4];

Adafruit_NeoPixel pixels(NUM_PIXELS, PIXEL_PIN, NEO_GRB + NEO_KHZ800);

// Create a instance of class LSM6DS3
LSM6DS3 my_imu(I2C_MODE, 0x6A);  // I2C device address 0x6A

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
  Serial.println("Neopixel BLE waking");

  if (my_imu.begin() != 0) {
    Serial.println("IMU error");
  }

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

  // set the initial value for the characteristic:
  pixel_srv.num_pixels_chr.writeValue(NUM_PIXELS);
  pixel_srv.num_colors_chr.writeValue(2U);
  pixel_srv.color01_chr.writeValue(PALETTE_INFO);
  pixel_srv.color02_chr.writeValue(PALETTE_LIME);

  digitalWrite(PIXEL_PIN, LOW);
  pixels.begin();  // INITIALIZE NeoPixel strip object (REQUIRED)
  pixels.setBrightness(20);
  pixels.show();  // Turn OFF all pixels

  // init color buffer
  for (size_t i = 0; i < NUM_PIXELS; i++) {
    current_colors[i] = 0;
  }

  color::blend(goal_colors, palette, NUM_PIXELS, PALETTE_INFO, PALETTE_LIME);
  transition_completed = false;
  milli_sec = millis();
  clock_sec = milli_sec * 1.0e-3f;
  transition_start_sec = clock_sec;
}

void loop() {
  milli_sec = millis();
  clock_sec = milli_sec * 1.0e-3f;
  pixel_srv.timer_chr.writeValue(milli_sec);
#ifdef SEEED_XIAO_NRF52840_SENSE
  digitalWrite(LEDG, !digitalRead(LEDG));  // heartbeats
#endif
  BLE.poll();
  // color parameter was changed, updating static colors
  if (pixel_srv.color01_chr.written() || pixel_srv.color02_chr.written() ||
      pixel_srv.color03_chr.written() || pixel_srv.color04_chr.written() ||
      pixel_srv.blending_chr.written() || pixel_srv.num_colors_chr.written()) {
// re-blend static color
#ifdef SEEED_XIAO_NRF52840_SENSE
    digitalWrite(LEDB, !digitalRead(LEDB));  // response for ble value changing
#endif
    transition_start_sec = clock_sec;
    // assign palette
    palette[0] = pixel_srv.color01_chr.value();
    palette[1] = pixel_srv.color02_chr.value();
    palette[2] = pixel_srv.color03_chr.value();
    palette[3] = pixel_srv.color04_chr.value();

    color::blend(goal_colors, palette, NUM_PIXELS,
                 pixel_srv.num_colors_chr.value(),
                 pixel_srv.blending_chr.value());

    transition_completed = false;

    for (size_t i = 0; i < NUM_PIXELS; i++) {
      start_colors[i] = current_colors[i];
    }
  }
  if (transition_completed == false) {
    transition_completed = color::wipeEasing(
        current_colors, start_colors, goal_colors, NUM_PIXELS,
        (clock_sec - transition_start_sec) / transition_duration, false);
  } else {
    // transition was completed
    // add noise/fluctuation
  }

  //  apply current color
  for (uint16_t i = 0; i < pixels.numPixels(); i++) {
    pixels.setPixelColor(i, current_colors[i]);
  }
  pixels.show();
  delay(DELAY_LED);

  // ble
  // BLEDevice central = BLE.central();
  // // if a central is connected to peripheral:
  // if (central) {
  //   digitalWrite(LEDB, HIGH);  // turn off
  //   digitalWrite(LEDG, LOW);   // turn on
  //   // print the central's MAC address:
  //   sprintf(message, "Connected to central: %s", central.address().c_str());
  //   Serial.println(message);

  // }
  // end of ble

  // // if a central is connected to peripheral:
  // if (central) {
  //   digitalWrite(LEDB, HIGH);
  //   digitalWrite(LEDG, LOW);
  //   // print the central's MAC address:
  //   sprintf(message, "Connected to central: %s", central.address().c_str());
  //   Serial.println(message);

  //   // while the central is still connected to peripheral:
  //   while (central.connected()) {
  //     // if the remote device wrote to the characteristic,
  //     // use the value to control the LED:
  //     if (switch_characteristic.written()) {
  //       progress = 0;
  //       completed = false;
  //       mode = switch_characteristic.valueLE();
  //       sprintf(message, "Receive new value: 0x%04x", mode);
  //       Serial.println(message);

  //       act = (0x0f00 & mode) >> 8;
  //       sprintf(message, "Action: 0x%01x", act);
  //       Serial.println(message);

  //       backward = ((0xf000 & mode) >> 12) > 0;

  //       unsigned char color_code1 = mode & 0x0F;
  //       unsigned char color_code2 = (mode & 0xF0) >> 4;

  //       sprintf(message, "color: 0x%01x -- 0x%01x", color_code1,
  //       color_code2); Serial.println(message);

  //       gradientColor(goal_colors, NUM_PIXELS, PALETTE[color_code1],
  //                     PALETTE[color_code2]);

  //       for (size_t i = 0; i < NUM_PIXELS; i++) {
  //         start_colors[i] = current_colors[i];
  //       }
  //     }

  //     // led strip update
  //     if (completed == false) {
  //       unsigned short duration = 30;
  //       u_int32_t cc = 0;

  //       switch (act) {
  //         case PRESET_ACT_WIPE: {
  //           completed = wipeUpdate(current_colors, start_colors, goal_colors,
  //                                  NUM_PIXELS, progress, !backward);
  //           // progress ratio = progress / (NUM_PIXELS - 1)
  //         } break;

  //         case PRESET_ACT_SLIDE: {
  //           completed = slideUpdate(current_colors, start_colors,
  //           goal_colors,
  //                                   NUM_PIXELS, progress);
  //         } break;

  //         case PRESET_ACT_DISSOLVE: {
  //           duration = 30;
  //           completed =
  //               dissolveUpdate(current_colors, start_colors, goal_colors,
  //                              NUM_PIXELS, progress, duration);
  //         } break;
  //         case PRESET_ACT_ACC: {  // imu acc
  //           float x = my_imu.readFloatAccelX();
  //           float y = my_imu.readFloatAccelY();
  //           float z = my_imu.readFloatAccelZ();

  //           duration = 10;

  //           unsigned int cx = 255 * abs(x / 2.0f);
  //           unsigned int cy = 255 * abs(y / 2.0f);
  //           unsigned int cz = 255 * abs(z / 2.0f);
  //           cc = (cx << 16 | cy << 8 | cz);

  //           if (progress < duration) {
  //             gradientColor(goal_colors, NUM_PIXELS, cc, cc);
  //             dissolveUpdate(current_colors, start_colors, goal_colors,
  //                            NUM_PIXELS, progress, duration);
  //           } else {
  //             gradientColor(current_colors, NUM_PIXELS, cc, cc);
  //           }
  //         }

  //         break;

  //         case PRESET_ACT_RAINBOW: {
  //           duration = 30;
  //           rainbowColor(goal_colors, NUM_PIXELS);
  //           completed =
  //               dissolveUpdate(current_colors, start_colors, goal_colors,
  //                              NUM_PIXELS, progress, duration);
  //         } break;

  //         default:
  //           break;
  //       }

  //       // apply current color
  //       for (int i = 0; i < pixels.numPixels(); i++) {
  //         uint32_t c = current_colors[i];
  //         pixels.setPixelColor(i, c);
  //       }
  //       pixels.show();

  //       progress += 1;

  //       if (completed) {
  //         sprintf(message, "0x%04x completed (%04d frames)", mode, progress);
  //         Serial.println(message);
  //       }
  //     }

  //     delay(DELAY_LED);
  //   }
  //   Serial.println("Disconnection");
  //   digitalWrite(LEDG, HIGH);
  // }
}

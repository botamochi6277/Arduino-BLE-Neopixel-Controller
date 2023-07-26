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
float transition_progress = 0.0f;

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

  for (size_t i = 0; i < 100; i++) {
    if (Serial) {
      break;
    }
    delay(100);
  }

  Serial.println("NeoPixel BLE waking");

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
  pixel_srv.transition_chr.writeValue(TRANSITION_DISSOLVE);
  pixel_srv.color01_chr.writeValue(color::hsbToHsbhex(0xff00, 0xff, 0xff));
  pixel_srv.color02_chr.writeValue(color::hsbToHsbhex(128U * 255U, 255U, 255U));

  digitalWrite(PIXEL_PIN, LOW);
  pixels.begin();  // INITIALIZE NeoPixel strip object (REQUIRED)
  pixels.setBrightness(20);
  pixels.show();  // Turn OFF all pixels

  // init color buffer
  for (size_t i = 0; i < NUM_PIXELS; i++) {
    current_colors[i] = 0;
  }
  palette[0] = pixel_srv.color01_chr.value();
  palette[1] = pixel_srv.color02_chr.value();
  palette[2] = pixel_srv.color03_chr.value();
  palette[3] = pixel_srv.color04_chr.value();

  color::blend(goal_colors, palette, NUM_PIXELS,
               pixel_srv.num_colors_chr.value(),
               pixel_srv.blending_chr.value());
  sprintf(message, "goal color: #%06x--#%06x", goal_colors[0],
          goal_colors[NUM_PIXELS - 1]);
  Serial.println(message);
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

    Serial.print("BLE params were updated, transition ");
    Serial.println(pixel_srv.transition_chr.value());

    transition_start_sec = clock_sec;
    // assign palette
    palette[0] = pixel_srv.color01_chr.value();
    palette[1] = pixel_srv.color02_chr.value();
    palette[2] = pixel_srv.color03_chr.value();
    palette[3] = pixel_srv.color04_chr.value();

    color::blend(goal_colors, palette, NUM_PIXELS,
                 pixel_srv.num_colors_chr.value(),
                 pixel_srv.blending_chr.value());
    sprintf(message, "goal color: $%08x--$%08x", goal_colors[0],
            goal_colors[NUM_PIXELS - 1]);
    Serial.println(message);

    transition_completed = false;

    for (size_t i = 0; i < NUM_PIXELS; i++) {
      start_colors[i] = current_colors[i];
    }
  }

  transition_progress =
      (clock_sec - transition_start_sec) / transition_duration;

  if (transition_completed == false) {
    switch (pixel_srv.transition_chr.value()) {
      case TRANSITION_DISSOLVE:
        transition_completed =
            color::dissolveEasing(current_colors, start_colors, goal_colors,
                                  NUM_PIXELS, transition_progress);
        break;
      case TRANSITION_WIPE:
        transition_completed =
            color::wipeEasing(current_colors, start_colors, goal_colors,
                              NUM_PIXELS, transition_progress, false);
        break;
      case TRANSITION_SLIDE:
        transition_completed =
            color::slideEasing(current_colors, start_colors, goal_colors,
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
    }

    for (uint16_t i = 0; i < pixels.numPixels(); i++) {
      pixels.setPixelColor(i, color::hsbhexTohex(current_colors[i]));
    }
    pixels.show();

  } else {
    // transition was completed
    // add noise/fluctuation
  }

  //  apply current color
  // for (uint16_t i = 0; i < pixels.numPixels(); i++) {
  //   pixels.setPixelColor(i, current_colors[i]);
  // }
  // pixels.show();
  delay(DELAY_LED);
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

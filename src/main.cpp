// base code
// https://github.com/adafruit/Adafruit_NeoPixel/blob/master/examples/StrandtestArduinoBLE/StrandtestArduinoBLE.ino

//#include "FastLED.h" // FastLED unsupports nrf52 series
// https://github.com/adafruit/Adafruit_NeoPixel
#include <Adafruit_NeoPixel.h>
#include <Arduino.h>
#include <ArduinoBLE.h>

#include "LSM6DS3.h"  // IMU chip
#include "Wire.h"
#include "color_utils.hpp"
#include "preset.hpp"
#include "strip.hpp"
// Neopixel variables
#define PIXEL_PIN 7  // MOSI
#define NUM_PIXELS 45
#define DELAY_MS 500

#define DELAY_LED 100

char message[128];  // buffer for sprintf

// mode:
//    act_opt act color2 color1
// 0x | 0  | 0  | 0     |  0
unsigned short mode = 0x0000;
unsigned int progress = 0;
bool completed = false;  // flag for color transition
unsigned char act;
bool backward;
unsigned int current_colors[NUM_PIXELS];
unsigned int start_colors[NUM_PIXELS];
unsigned int goal_colors[NUM_PIXELS];

Adafruit_NeoPixel pixels(NUM_PIXELS, PIXEL_PIN, NEO_GRB + NEO_KHZ800);

// Create a instance of class LSM6DS3
LSM6DS3 my_imu(I2C_MODE, 0x6A);  // I2C device address 0x6A

// BLE Variables

BLEService led_service(
    "19B10000-E8F2-537E-4F6C-D104768A1214");  // BLE LED Service

BLEUnsignedLongCharacteristic timer_characteristic(
    "19B10001-E8F2-537E-4F6C-D104768A1214", BLERead | BLENotify);
//  Characteristic User Description Descriptor
BLEDescriptor timer_descriptor("2901", "timer_ms");

// BLE LED Switch Characteristic - custom 128-bit UUID, read and writable by
BLEUnsignedShortCharacteristic switch_characteristic(
    "19B10002-E8F2-537E-4F6C-D104768A1214", BLERead | BLEWrite);
BLEDescriptor led_descriptor("2901", "action_color");

void setup() {
  Serial.begin(115200);
  Serial.println("Neopixel BLE waking");

  if (my_imu.begin() != 0) {
    Serial.println("IMU error");
  }

  // custom services and characteristics can be added as well
  // begin initialization
  if (!BLE.begin()) {
    Serial.println("starting BLE failed!");

    pinMode(LEDR, OUTPUT);
    while (1) {
      digitalWrite(LEDR, !digitalRead(LEDR));
      delay(100);
    }
  }

  Serial.print("Peripheral address: ");
  Serial.println(BLE.address());

  // set advertised local name and service UUID:
  BLE.setDeviceName("ArduinoNeopixels");
  BLE.setLocalName("Neopixels");
  BLE.setAdvertisedService(led_service);

  // add the characteristic to the service
  led_service.addCharacteristic(switch_characteristic);
  switch_characteristic.addDescriptor(led_descriptor);
  // add service
  BLE.addService(led_service);

  // set the initial value for the characteristic:
  switch_characteristic.writeValue(0x0111);

  // start advertising
  BLE.advertise();

  // build-in leds
  pinMode(LEDR, OUTPUT);
  pinMode(LEDG, OUTPUT);
  pinMode(LEDB, OUTPUT);
  // turn off the all build-in leds
  digitalWrite(LEDR, HIGH);
  digitalWrite(LEDG, HIGH);
  digitalWrite(LEDB, HIGH);

  pinMode(PIXEL_PIN, OUTPUT);
  digitalWrite(PIXEL_PIN, LOW);
  pixels.begin();  // INITIALIZE NeoPixel strip object (REQUIRED)
  pixels.show();   // Turn OFF all pixels ASAP

  for (size_t i = 0; i < NUM_PIXELS; i++) {
    start_colors[i] = current_colors[i];
  }
  gradientColor(goal_colors, NUM_PIXELS, PALETTE_INFO, PALETTE_LIME);
  unsigned int duration = 10;
  for (size_t i = 0; i < duration; i++) {
    dissolveUpdate(current_colors, start_colors, goal_colors, NUM_PIXELS,
                   progress, duration);
    for (int i = 0; i < pixels.numPixels(); i++) {
      // uint32_t c = current_colors[i];
      pixels.setPixelColor(i, (uint32_t)current_colors[i]);
    }
    progress += 1;
    pixels.show();
    delay(DELAY_LED);
  }
  progress = 0;
  completed = true;
}

void loop() {
  digitalWrite(LEDB, !digitalRead(LEDB));  // waiting for connection

  BLEDevice central = BLE.central();

  // if a central is connected to peripheral:
  if (central) {
    digitalWrite(LEDB, HIGH);
    digitalWrite(LEDG, LOW);
    // print the central's MAC address:
    sprintf(message, "Connected to central: %s", central.address().c_str());
    Serial.println(message);

    // while the central is still connected to peripheral:
    while (central.connected()) {
      // if the remote device wrote to the characteristic,
      // use the value to control the LED:
      if (switch_characteristic.written()) {
        progress = 0;
        completed = false;
        mode = switch_characteristic.valueLE();
        sprintf(message, "Receive new value: 0x%04x", mode);
        Serial.println(message);

        act = (0x0f00 & mode) >> 8;
        sprintf(message, "Action: 0x%01x", act);
        Serial.println(message);

        backward = ((0xf000 & mode) >> 12) > 0;

        unsigned char color_code1 = mode & 0x0F;
        unsigned char color_code2 = (mode & 0xF0) >> 4;

        sprintf(message, "color: 0x%01x -- 0x%01x", color_code1, color_code2);
        Serial.println(message);

        gradientColor(goal_colors, NUM_PIXELS, PALETTE[color_code1],
                      PALETTE[color_code2]);

        for (size_t i = 0; i < NUM_PIXELS; i++) {
          start_colors[i] = current_colors[i];
        }
      }

      // led strip update
      if (completed == false) {
        unsigned short duration = 30;
        u_int32_t cc = 0;

        switch (act) {
          case PRESET_ACT_WIPE: {
            completed = wipeUpdate(current_colors, start_colors, goal_colors,
                                   NUM_PIXELS, progress, !backward);
            // progress ratio = progress / (NUM_PIXELS - 1)
          } break;

          case PRESET_ACT_SLIDE: {
            completed = slideUpdate(current_colors, start_colors, goal_colors,
                                    NUM_PIXELS, progress);
          } break;

          case PRESET_ACT_DISSOLVE: {
            duration = 30;
            completed =
                dissolveUpdate(current_colors, start_colors, goal_colors,
                               NUM_PIXELS, progress, duration);
          } break;
          case PRESET_ACT_ACC: {  // imu acc
            float x = my_imu.readFloatAccelX();
            float y = my_imu.readFloatAccelY();
            float z = my_imu.readFloatAccelZ();

            duration = 10;

            unsigned int cx = 255 * abs(x / 2.0f);
            unsigned int cy = 255 * abs(y / 2.0f);
            unsigned int cz = 255 * abs(z / 2.0f);
            cc = (cx << 16 | cy << 8 | cz);

            if (progress < duration) {
              gradientColor(goal_colors, NUM_PIXELS, cc, cc);
              dissolveUpdate(current_colors, start_colors, goal_colors,
                             NUM_PIXELS, progress, duration);
            } else {
              gradientColor(current_colors, NUM_PIXELS, cc, cc);
            }
          }

          break;

          case PRESET_ACT_RAINBOW: {
            duration = 30;
            rainbowColor(goal_colors, NUM_PIXELS);
            completed =
                dissolveUpdate(current_colors, start_colors, goal_colors,
                               NUM_PIXELS, progress, duration);
          } break;

          default:
            break;
        }

        // apply current color
        for (int i = 0; i < pixels.numPixels(); i++) {
          uint32_t c = current_colors[i];
          pixels.setPixelColor(i, c);
        }
        pixels.show();

        progress += 1;

        if (completed) {
          sprintf(message, "0x%04x completed (%04d frames)", mode, progress);
          Serial.println(message);
        }
      }

      delay(DELAY_LED);
    }
    Serial.println("Disconnection");
    digitalWrite(LEDG, HIGH);
  }

  delay(DELAY_MS);
}

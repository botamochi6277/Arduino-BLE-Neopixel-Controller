// base code
// https://github.com/adafruit/Adafruit_NeoPixel/blob/master/examples/StrandtestArduinoBLE/StrandtestArduinoBLE.ino

//#include "FastLED.h" // FastLED unsupports nrf52 series
// https://github.com/adafruit/Adafruit_NeoPixel
#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include <ArduinoBLE.h>

#include "strip.hpp"
#include "color_utils.hpp"
#include "preset.hpp"
// Neopixel veriables
#define PIXEL_PIN 10 // MOSI
#define NUM_PIXELS 45
#define DELAYVAL 500

#define DELAY_LED 100

char message[128]; // buffer for sprintf

// mode:
//      act   color2 color1
// 0x | 00  | 0     |  0
unsigned short mode = 0x0000;
unsigned int progress = 0;
bool completed = false;
unsigned char act;
unsigned int current_colors[NUM_PIXELS];
unsigned int start_colors[NUM_PIXELS];
unsigned int goal_colors[NUM_PIXELS];

Adafruit_NeoPixel pixels(NUM_PIXELS, PIXEL_PIN, NEO_GRB + NEO_KHZ800);

// BLE Variables

BLEService ledService("19B10000-E8F2-537E-4F6C-D104768A1214"); // BLE LED Service

// BLE LED Switch Characteristic - custom 128-bit UUID, read and writable by central
BLEUnsignedShortCharacteristic switchCharacteristic("19B10001-E8F2-537E-4F6C-D104768A1214", BLERead | BLEWrite);

void setup()
{
  Serial.begin(115200);
  Serial.println("Neopixel BLE waking");

  // custom services and characteristics can be added as well
  // begin initialization
  if (!BLE.begin())
  {
    Serial.println("starting BLE failed!");

    pinMode(LEDR, OUTPUT);
    while (1)
    {
      digitalWrite(LEDR, !digitalRead(LEDR));
      delay(100);
    }
  }

  Serial.print("Peripheral address: ");
  Serial.println(BLE.address());

  // set advertised local name and service UUID:
  BLE.setDeviceName("ArduinoNeopixels");
  BLE.setLocalName("Neopixels");
  BLE.setAdvertisedService(ledService);

  // add the characteristic to the service
  ledService.addCharacteristic(switchCharacteristic);

  // add service
  BLE.addService(ledService);

  // set the initial value for the characteristic:
  switchCharacteristic.writeValue(0x0111);

  // start advertising
  BLE.advertise();

  pinMode(LEDB, OUTPUT); // for ble heartbeat
  pinMode(PIXEL_PIN, OUTPUT);
  digitalWrite(PIXEL_PIN, LOW);
  pixels.begin(); // INITIALIZE NeoPixel strip object (REQUIRED)
  pixels.show();  // Turn OFF all pixels ASAP

  colorWipe(pixels, pixels.Color(0, 200, 200), 20); // Wakeup lighting
}

void loop()
{
  digitalWrite(LEDB, !digitalRead(LEDB));

  BLEDevice central = BLE.central();

  // if a central is connected to peripheral:
  if (central)
  {
    // print the central's MAC address:
    sprintf(message, "Connected to central: %s", central.address().c_str());
    Serial.println(message);

    // while the central is still connected to peripheral:
    while (central.connected())
    {
      // if the remote device wrote to the characteristic,
      // use the value to control the LED:
      if (switchCharacteristic.written())
      {
        progress = 0;
        completed = false;
        mode = switchCharacteristic.valueLE();
        sprintf(message, "Recieve new value: 0x%04x", mode);
        Serial.println(message);

        act = mode >> 8;
        sprintf(message, "Action: 0x%02x", act);
        Serial.println(message);

        unsigned char color_code1 = mode & 0x0F;
        unsigned char color_code2 = (mode & 0xF0) >> 4;

        sprintf(message, "color: 0x%01x -- 0x%01x", color_code1, color_code2);
        Serial.println(message);

        gradient_color(
            goal_colors, NUM_PIXELS,
            PALETTE[color_code1], PALETTE[color_code2]);

        // memcpy(start_colors, current_colors, sizeof(current_colors));

        for (size_t i = 0; i < NUM_PIXELS; i++)
        {
          start_colors[i] = current_colors[i];
        }
      }

      if (completed == false)
      {
        switch (act)
        {
        case PRESET_ACT_WIPE:
          wipeUpdate(
              current_colors, start_colors, goal_colors,
              NUM_PIXELS, progress, false);
          // complete check
          completed = (progress == (NUM_PIXELS - 1));
          break;

        case PRESET_ACT_SLIDE:
          slideUpdate(
              current_colors, start_colors, goal_colors,
              NUM_PIXELS, progress);
          completed = (progress == NUM_PIXELS);
          break;

        case PRESET_ACT_DISSOLVE:
          unsigned short duration = 30;
          dissolveUpdate(current_colors, start_colors, goal_colors,
                         NUM_PIXELS, progress, duration);
          completed = (progress == duration);

          break;
        }

        for (int i = 0; i < pixels.numPixels(); i++)
        {
          uint32_t c = current_colors[i];
          pixels.setPixelColor(i, c);
        }
        pixels.show();

        progress += 1;

        if (completed)
        {
          sprintf(message, "0x%04x completed", mode);
          Serial.println(message);
        }
      }

      delay(DELAY_LED);
    }
    Serial.println("Disconnection");
  }

  delay(DELAYVAL);
}

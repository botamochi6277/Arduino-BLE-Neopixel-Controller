// base code
// https://github.com/adafruit/Adafruit_NeoPixel/blob/master/examples/StrandtestArduinoBLE/StrandtestArduinoBLE.ino

//#include "FastLED.h" // FastLED unsupports nrf52 series
// https://github.com/adafruit/Adafruit_NeoPixel
#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include <ArduinoBLE.h>

#include "strip.hpp"

// Neopixel veriables
#define PIXEL_PIN 10 // MOSI
#define NUM_PIXELS 45
#define DELAYVAL 500

Adafruit_NeoPixel pixels(NUM_PIXELS, PIXEL_PIN, NEO_GRB + NEO_KHZ800);

// BLE Variables

BLEService ledService("19B10000-E8F2-537E-4F6C-D104768A1214"); // BLE LED Service

// BLE LED Switch Characteristic - custom 128-bit UUID, read and writable by central
BLEByteCharacteristic switchCharacteristic("19B10001-E8F2-537E-4F6C-D104768A1214", BLERead | BLEWrite);

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
  BLE.setLocalName("Neopixels");
  BLE.setAdvertisedService(ledService);

  // add the characteristic to the service
  ledService.addCharacteristic(switchCharacteristic);

  // add service
  BLE.addService(ledService);

  // set the initial value for the characeristic:
  switchCharacteristic.writeValue(0);

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
    Serial.print("Connected to central: ");
    // print the central's MAC address:
    Serial.println(central.address());

    // while the central is still connected to peripheral:
    while (central.connected())
    {
      // if the remote device wrote to the characteristic,
      // use the value to control the LED:
      if (switchCharacteristic.written())
      {
        switch (switchCharacteristic.value())
        {
        case 0x00:
          colorWipe(pixels, pixels.Color(0, 0, 0), 20); // Clear
          break;
        case 0x0a:
          colorWipe(pixels, pixels.Color(255, 0, 0), 20); // Red
          break;
        case 0x0b:
          colorWipe(pixels, pixels.Color(0, 255, 0), 20); // Green
          break;
        case 0x0c:
          colorWipe(pixels, pixels.Color(0, 0, 255), 20); // Blue
          break;
        case 0x0d:
          theaterChase(pixels, pixels.Color(255, 0, 0), 20); // Red
          break;
        case 0x0e:
          theaterChase(pixels, pixels.Color(0, 255, 0), 20); // Green
          break;
        case 0x0f:
          theaterChase(pixels, pixels.Color(255, 0, 255), 20); // Cyan
          break;
        case 0x01:
          rainbow(pixels, 5);
          break;
        case 0x02:
          theaterChaseRainbow(pixels, 20);
          break;
        }
      }
    }
  }

  delay(DELAYVAL);
}

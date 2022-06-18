// base code
// https://github.com/adafruit/Adafruit_NeoPixel/blob/master/examples/StrandtestArduinoBLE/StrandtestArduinoBLE.ino

//#include "FastLED.h" // FastLED unsupports nrf52 series
// https://github.com/adafruit/Adafruit_NeoPixel
#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include <ArduinoBLE.h>

// Neopixel veriables
#define PIXEL_PIN 10 // MOSI
#define NUM_PIXELS 45
#define DELAYVAL 500

Adafruit_NeoPixel pixels(NUM_PIXELS, PIXEL_PIN, NEO_GRB + NEO_KHZ800);

// BLE Variables

BLEService ledService("19B10000-E8F2-537E-4F6C-D104768A1214"); // BLE LED Service

// BLE LED Switch Characteristic - custom 128-bit UUID, read and writable by central
BLEByteCharacteristic switchCharacteristic("19B10001-E8F2-537E-4F6C-D104768A1214", BLERead | BLEWrite);

void colorWipe(Adafruit_NeoPixel &strip, uint32_t color, int wait);
void theaterChase(Adafruit_NeoPixel &strip, uint32_t color, int wait);
void rainbow(Adafruit_NeoPixel &strip, int wait);
void theaterChaseRainbow(Adafruit_NeoPixel &strip, int wait);

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
          rainbow(pixels, 10);
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

// Fill strip pixels one after another with a color. Strip is NOT cleared
// first; anything there will be covered pixel by pixel. Pass in color
// (as a single 'packed' 32-bit value, which you can get by calling
// strip.Color(red, green, blue) as shown in the loop() function above),
// and a delay time (in milliseconds) between pixels.
void colorWipe(Adafruit_NeoPixel &strip, uint32_t color, int wait)
{
  for (int i = 0; i < strip.numPixels(); i++)
  {                                // For each pixel in strip...
    strip.setPixelColor(i, color); //  Set pixel's color (in RAM)
    strip.show();                  //  Update strip to match
    delay(wait);                   //  Pause for a moment
  }
}

// Theater-marquee-style chasing lights. Pass in a color (32-bit value,
// a la strip.Color(r,g,b) as mentioned above), and a delay time (in ms)
// between frames.
void theaterChase(Adafruit_NeoPixel &strip, uint32_t color, int wait)
{
  for (int a = 0; a < 10; a++)
  { // Repeat 10 times...
    for (int b = 0; b < 3; b++)
    {                //  'b' counts from 0 to 2...
      strip.clear(); //   Set all pixels in RAM to 0 (off)
      // 'c' counts up from 'b' to end of strip in steps of 3...
      for (int c = b; c < strip.numPixels(); c += 3)
      {
        strip.setPixelColor(c, color); // Set pixel 'c' to value 'color'
      }
      strip.show(); // Update strip with new contents
      delay(wait);  // Pause for a moment
    }
  }
}

// Rainbow cycle along whole strip. Pass delay time (in ms) between frames.
void rainbow(Adafruit_NeoPixel &strip, int wait)
{
  // Hue of first pixel runs 5 complete loops through the color wheel.
  // Color wheel has a range of 65536 but it's OK if we roll over, so
  // just count from 0 to 5*65536. Adding 256 to firstPixelHue each time
  // means we'll make 5*65536/256 = 1280 passes through this outer loop:
  for (long firstPixelHue = 0; firstPixelHue < 5 * 65536; firstPixelHue += 256)
  {
    for (int i = 0; i < strip.numPixels(); i++)
    { // For each pixel in strip...
      // Offset pixel hue by an amount to make one full revolution of the
      // color wheel (range of 65536) along the length of the strip
      // (strip.numPixels() steps):
      int pixelHue = firstPixelHue + (i * 65536L / strip.numPixels());
      // strip.ColorHSV() can take 1 or 3 arguments: a hue (0 to 65535) or
      // optionally add saturation and value (brightness) (each 0 to 255).
      // Here we're using just the single-argument hue variant. The result
      // is passed through strip.gamma32() to provide 'truer' colors
      // before assigning to each pixel:
      strip.setPixelColor(i, strip.gamma32(strip.ColorHSV(pixelHue)));
    }
    strip.show(); // Update strip with new contents
    delay(wait);  // Pause for a moment
  }
}

// Rainbow-enhanced theater marquee. Pass delay time (in ms) between frames.
void theaterChaseRainbow(Adafruit_NeoPixel &strip, int wait)
{
  int firstPixelHue = 0; // First pixel starts at red (hue 0)
  for (int a = 0; a < 30; a++)
  { // Repeat 30 times...
    for (int b = 0; b < 3; b++)
    {                //  'b' counts from 0 to 2...
      strip.clear(); //   Set all pixels in RAM to 0 (off)
      // 'c' counts up from 'b' to end of strip in increments of 3...
      for (int c = b; c < strip.numPixels(); c += 3)
      {
        // hue of pixel 'c' is offset by an amount to make one full
        // revolution of the color wheel (range 65536) along the length
        // of the strip (strip.numPixels() steps):
        int hue = firstPixelHue + c * 65536L / strip.numPixels();
        uint32_t color = strip.gamma32(strip.ColorHSV(hue)); // hue -> RGB
        strip.setPixelColor(c, color);                       // Set pixel 'c' to value 'color'
      }
      strip.show();                // Update strip with new contents
      delay(wait);                 // Pause for a moment
      firstPixelHue += 65536 / 90; // One cycle of color wheel over 90 frames
    }
  }
}

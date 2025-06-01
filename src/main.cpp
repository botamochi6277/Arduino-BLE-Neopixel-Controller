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
#include <JC_Button.h>
#include <TaskManager.h>
#include <Wire.h>

#include "BLEPresets.hpp"
#include "ColorUtils.hpp"
#include "Colormap.hpp"
#include "DataSource.hpp"
#include "MyTasks.hpp"
#include "MyUtils.hpp"
#include "NeopixelService.hpp"

// define pins
#ifdef ARDUINO_M5Stack_ATOM
#define PIXEL_PIN 32  // MOSI
#define BTN_A_PIN 39  // button A
#define BTN_B_PIN 33  // button B
#else
#define PIXEL_PIN 7  // MOSI
#define BTN_A_PIN 4  // button A
#define BTN_B_PIN 5  // button B
#endif

#define REED_PIN 1
#define NUM_PIXELS 45
#define DELAY_MS 500

#define DELAY_LED 100

char message[128];  // buffer for sprintf

ble::NeoPixelService pixel_srv;

unsigned long milli_sec;
float clock_sec;
unsigned int loop_count = 0;

bool transition_completed = false;  // flag for color transition
bool backward;

float magnitude = 0.0f;
float intensity[NUM_PIXELS];
color::PixelUnit color_caches[NUM_PIXELS];  // color cache
Adafruit_NeoPixel pixels(NUM_PIXELS, PIXEL_PIN, NEO_GRB + NEO_KHZ800);

// Debounce time 10ms, pullup
Button button_a(BTN_A_PIN, true, 10);
Button button_b(BTN_B_PIN, true, 10);
uint8_t cmap_id = 0;
uint8_t shape_id = 0;
uint8_t input_id = 0;

#ifdef LSM6DS3_ENABLED
// Create a instance of class LSM6DS3
LSM6DS3 my_imu(I2C_MODE, 0x6A);  // I2C device address 0x6A
float last_knock_time = 0.0f;
float min_knock_interval = 1.0f;
#endif

void setup() {
#ifdef LSM6DS3_ENABLED
    init_leds(LEDR, LEDG, LEDB);
#endif
    pinMode(PIXEL_PIN, OUTPUT);

    // reed switch
    pinMode(REED_PIN, INPUT_PULLUP);

    Serial.begin(115200);

    for (size_t i = 0; i < 100; i++) {
        if (Serial) {
            break;
        }
        delay(10);
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
    pixel_srv.init(50U, NUM_PIXELS,
                   static_cast<uint8_t>(data_source::DataSource::kBeatSin05),
                   static_cast<uint8_t>(shape::IntensityFuncId::kSawWave),
                   static_cast<uint8_t>(colormap::ColormapId::kHsv));
    tasks::reflectParams(pixel_srv, pixels, false);  // reflect initial values

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

    digitalWrite(PIXEL_PIN, LOW);
    pixels.begin();  // INITIALIZE NeoPixel strip object (REQUIRED)
    pixels.setBrightness(pixel_srv.brightness_chr.value());
    pixels.show();  // Turn OFF all pixels

    loop_count = 0;
    // register tasks
    Tasks
        .add("BLE_polling",
             [] {
                 BLE.poll();
                 tasks::reflectParams(pixel_srv, pixels);
                 pixel_srv.magnitude_chr.writeValueLE(magnitude);
#ifdef SEEED_XIAO_NRF52840_SENSE
                 digitalWrite(LEDB, !digitalRead(LEDB));
#endif
             })
        ->startFps(10);
    // clock publishing task
    Tasks
        .add("Clock",
             [] {
                 milli_sec = millis();
                 clock_sec = milli_sec * 1.0e-3f;
                 pixel_srv.timer_chr.writeValue(milli_sec);
             })
        ->startFps(60);

#ifdef SEEED_XIAO_NRF52840_SENSE
    Tasks.add("Heart_beats", [] { digitalWrite(LEDG, !digitalRead(LEDG)); })
        ->startFps(1.0);
#endif

    Tasks
        .add("UpdateColorCache",
             [] {
                 // update magnitude
                 auto src_label = static_cast<data_source::DataSource>(
                     pixel_srv.input_chr.value());
                 magnitude = data_source::getSrcValue(src_label);
#ifdef LSM6DS3_ENABLED
                 if (magnitude < 1e-6f) {
                     magnitude = data_source::getSrcValue(src_label, my_imu);
                 }
#endif
                 // update intensity
                 shape::setIntensity(magnitude, intensity, NUM_PIXELS,
                                     static_cast<shape::IntensityFuncId>(
                                         pixel_srv.intensity_func_chr.value()),
                                     pixel_srv.wave_speed_chr.valueLE() < 0.0f);
                 // set color cache
                 for (size_t i = 0; i < NUM_PIXELS; i++) {
                     color_caches[i].setCmapColor(
                         intensity[i], static_cast<colormap::ColormapId>(
                                           pixel_srv.colormap_chr.value()));
                 }
             })
        ->startFps(24.0);

    Tasks
        .add("UpdatePixelColors",
             [] {
                 if (digitalRead(REED_PIN) == LOW) {
                     // reed switch is on
                     // turn off all pixels
                     pixels.fill(pixels.Color(0, 0, 0), 0, NUM_PIXELS);
                     pixels.show();
                 } else {
                     tasks::setPixelColors(color_caches, pixels);
                     pixels.show();
                 }
             })
        ->startFps(24.0);

#ifdef LSM6DS3_ENABLED
    Tasks
        .add("DetectKnock",
             [] {
                 if (pixel_srv.knock_activate_chr.value() < 1) {
                     return;
                 }

                 if (clock_sec - last_knock_time < min_knock_interval) {
                     return;
                 }
                 auto knock = tasks::detectKnock(my_imu, 1.8f);
                 if (knock == tasks::Knock::Undetected) {
                     return;
                 }
                 tasks::shiftChrValue(pixel_srv, knock);
                 last_knock_time = clock_sec;
             })
        ->startFps(30.0);
#endif
    button_a.begin();
    button_b.begin();

}  // end of setup

void loop() {
    milli_sec = millis();
    clock_sec = milli_sec * 1.0e-3f;
    pixel_srv.timer_chr.writeValue(milli_sec);
    Tasks.update();  // automatically execute tasks

    // TODO : add a task to update the button state
    button_a.read();
    button_b.read();
    if (button_a.wasPressed()) {
        // button pressed
        cmap_id += 1;
        if (cmap_id >= static_cast<uint8_t>(colormap::ColormapId::LENGTH)) {
            cmap_id = 0;
        }
        pixel_srv.colormap_chr.writeValue(cmap_id);
    }

    if (button_b.wasPressed()) {
        // button pressed
        shape_id += 1;
        if (shape_id >= static_cast<uint8_t>(shape::IntensityFuncId::LENGTH)) {
            // reset to 0
            shape_id = 0;
        }
        pixel_srv.intensity_func_chr.writeValue(shape_id);
    }

    if (button_a.wasPressed() && button_b.wasPressed()) {
        // button pressed
        input_id += 1;
        if (input_id >= static_cast<uint8_t>(data_source::DataSource::LENGTH)) {
            // reset to 0
            input_id = 0;
        }
        pixel_srv.input_chr.writeValue(input_id);
    }

    delay(10);
    loop_count++;
}

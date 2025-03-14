#ifndef BOTAMOCHI_DATA_SOURCE
#define BOTAMOCHI_DATA_SOURCE

#include <Arduino.h>
namespace data_source {
enum class DataSource : unsigned char {
    BeatSin05,  // sine beat 5Hz
    BeatSin10,
    BeatSin20,
    BeatSaw05,  // saw beat 5Hz
    BeatSaw10,  // saw beat 1.0Hz
    BeatSaw20,  // saw beat 2.0Hz
    AccX,
    AccY,
    AccZ,
    AccMAG,
    GyroX,
    GyroY,
    GyroZ,
    LENGTH
};
String input_name(DataSource id) {
    static String names[] = {
        "Beat/Sine/0.5Hz", "Beat/Sine/1.0Hz", "Beat/Sine/2.0Hz",
        "Beat/Saw/0.5Hz",  "Beat/Saw/1.0Hz",  "Beat/Saw/2.0Hz",
        "Acc/X",           "Acc/Y",           "Acc/Z",
        "Acc/Mag",         "Gyro/X",          "Gyro/Y",
        "Gyro/Z",
    };
    return names[static_cast<uint8_t>(id)];
}

float getSrcValue(DataSource input_src) {
    float magnitude = 0.0f;
    float t = (millis() / 1.0e3f);
    switch (input_src) {
        case DataSource::BeatSin05:
            // 0.5 Hz
            magnitude = 0.5f * sinf(2.0f * M_PI * 0.5f * t) + 0.5f;
            break;
        case DataSource::BeatSin10:
            // 1.0 Hz
            magnitude = 0.5 * sinf(2.0f * M_PI * 1.0f * t) + 0.5f;
            break;
        case DataSource::BeatSin20:
            // 2.0 Hz
            magnitude = 0.5 * sinf(2.0f * M_PI * 2.0f * t) + 0.5f;
            break;
        case DataSource::BeatSaw05:
            // 0.5 Hz
            magnitude = 0.5f * t - floorf(0.5f * t);
            break;
        case DataSource::BeatSaw10:
            // 1.0 Hz
            magnitude = t - floorf(t);
            break;
        case DataSource::BeatSaw20:
            // 0.5 Hz
            magnitude = 2.0f * t - floorf(2.0f * t);
            break;
        default:
            break;
    }
    return magnitude;
}

#ifdef LSM6DS3_ENABLED
float getSrcValue(DataSource input_src, LSM6DS3 &imu) {
    float magnitude = 0.0f;
    static float max_acc = 1.0f;
    switch (input_src) {
        case DataSource::AccX:
            magnitude = easing::remap(imu.readFloatAccelX(), -max_acc, max_acc,
                                      -1.0f, 1.0f, true);
            break;
        case DataSource::AccY:
            magnitude = easing::remap(imu.readFloatAccelY(), -max_acc, max_acc,
                                      -1.0f, 1.0f, true);
            break;
        case DataSource::AccZ:
            magnitude = easing::remap(imu.readFloatAccelZ(), -max_acc, max_acc,
                                      -1.0f, 1.0f, true);
            break;
        case DataSource::GyroX:
            magnitude = easing::remap(imu.readFloatGyroX(), -180.0f, 180.0f,
                                      -1.0f, 1.0f, true);
            break;
        case DataSource::GyroY:
            magnitude = easing::remap(imu.readFloatGyroY(), -180.0f, 180.0f,
                                      -1.0f, 1.0f, true);
            break;
        case DataSource::GyroZ:
            magnitude = easing::remap(imu.readFloatGyroZ(), -180.0f, 180.0f,
                                      -1.0f, 1.0f, true);
            break;
    }

    return magnitude;
}
#endif
}  // namespace data_source

#endif
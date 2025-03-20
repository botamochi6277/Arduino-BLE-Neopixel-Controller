#ifndef BOTAMOCHI_DATA_SOURCE
#define BOTAMOCHI_DATA_SOURCE

#include <Arduino.h>
namespace data_source {
enum class DataSource : unsigned char {
    kBeatSin05,  // sine beat 5Hz
    kBeatSin10,
    kBeatSin20,
    kBeatSaw05,  // saw beat 5Hz
    kBeatSaw10,  // saw beat 1.0Hz
    kBeatSaw20,  // saw beat 2.0Hz
    kAccX,
    kAccY,
    kAccZ,
    kAccMAG,
    kGyroX,
    kGyroY,
    kGyroZ,
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
        case DataSource::kBeatSin05:
            // 0.5 Hz
            magnitude = 0.5f * sinf(2.0f * M_PI * 0.5f * t) + 0.5f;
            break;
        case DataSource::kBeatSin10:
            // 1.0 Hz
            magnitude = 0.5 * sinf(2.0f * M_PI * 1.0f * t) + 0.5f;
            break;
        case DataSource::kBeatSin20:
            // 2.0 Hz
            magnitude = 0.5 * sinf(2.0f * M_PI * 2.0f * t) + 0.5f;
            break;
        case DataSource::kBeatSaw05:
            // 0.5 Hz
            magnitude = 0.5f * t - floorf(0.5f * t);
            break;
        case DataSource::kBeatSaw10:
            // 1.0 Hz
            magnitude = t - floorf(t);
            break;
        case DataSource::kBeatSaw20:
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
    static float max_acc = 1.5f;
    static float max_gyro = 180.0f;
    switch (input_src) {
        case DataSource::kAccX:
            magnitude = easing::remap(imu.readFloatAccelX(), -max_acc, max_acc,
                                      0.0f, 1.0f, true);
            break;
        case DataSource::kAccY:
            magnitude = easing::remap(imu.readFloatAccelY(), -max_acc, max_acc,
                                      0.0f, 1.0f, true);
            break;
        case DataSource::kAccZ:
            magnitude = easing::remap(imu.readFloatAccelZ(), -max_acc, max_acc,
                                      0.0f, 1.0f, true);
            break;
        case DataSource::kGyroX:
            magnitude = easing::remap(imu.readFloatGyroX(), -max_gyro, max_gyro,
                                      0.0f, 1.0f, true);
            break;
        case DataSource::kGyroY:
            magnitude = easing::remap(imu.readFloatGyroY(), -max_gyro, max_gyro,
                                      0.0f, 1.0f, true);
            break;
        case DataSource::kGyroZ:
            magnitude = easing::remap(imu.readFloatGyroZ(), -max_gyro, max_gyro,
                                      0.0f, 1.0f, true);
            break;
    }

    return magnitude;
}
#endif
}  // namespace data_source

#endif
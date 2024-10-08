#ifndef BLE_UNIT_HPP
#define BLE_UNIT_HPP

// https://btprodspecificationrefs.blob.core.windows.net/assigned-numbers/Assigned%20Number%20Types/Assigned_Numbers.pdf
// pp.78--81 0x2700--0x27c8 are assigned
#define BLE_GATT_CPF_UNIT_UNITLESS 0x2700
#define BLE_GATT_CPF_UNIT_METER 0x2701   // length(meter)
#define BLE_GATT_CPF_UNIT_KG 0x2702      // mass(kg)
#define BLE_GATT_CPF_UNIT_SEC 0x2703     // time(second)
#define BLE_GATT_CPF_UNIT_AMPERE 0x2704  // current intensity(ampere)
#define BLE_GATT_CPF_UNIT_KELVIN 0x2705  // temperature(kelvin)

#define BLE_GATT_CPF_UNIT_METER_PER_S 0x2712   // velocity(m/s)
#define BLE_GATT_CPF_UNIT_METER_PER_SS 0x2713  // acc(m/s^2)

#define BLE_GATT_CPF_UNIT_RAD 0x2720      // angle(radian)
#define BLE_GATT_CPF_UNIT_HERTZ 0x2722    // frequency(Hertz)
#define BLE_GATT_CPF_UNIT_CELSIUS 0x272F  // Celsius temperature

#define BLE_GATT_CPF_UNIT_RAD_PER_S 0x2743  // angular velocity (rad/s)

// custom
#define BLE_GATT_CPF_UNIT_RGB 0x27F0  // rgb
#define BLE_GATT_CPF_UNIT_HSB 0x27F1  // hsb
#endif
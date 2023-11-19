# XIAO BLE Neopixel Controller

[![PlatformIO Build](https://github.com/botamochi6277/XIAO-BLE-Neopixel-Controller/actions/workflows/ci-platformio.yml/badge.svg)](https://github.com/botamochi6277/XIAO-BLE-Neopixel-Controller/actions/workflows/ci-platformio.yml)

Control Neopixel-strip with [Seeeduino XIAO BLE](https://wiki.seeedstudio.com/XIAO_BLE/)

You can change the lighting colors of the pixels with BLE.

```mermaid
graph LR

subgraph BLE Service
  color01
  color02
  color03
  color04

  blend_type
  noise
end

blend1((+))

color01-->blend1
color02-->blend1
blend_type-->blend1

blend1-->c1([static pixel colors])

blend2((+))

c1-->blend2
noise-->blend2

blend2-->c2([dynamic pixel colors])
c2-->neopixels

```

## Service Profile

```yml
---
name: "NeopixelService"
uuid: "19B10000-E8F2-537E-4F6C-D104768A1214"
characteristics:
  - name: "timer_chr"
    uuid: "19B10001-E8F2-537E-4F6C-D104768A1214"
    data_type: "UnsignedLong"
    properties:
      - "Read"
      - "Notify"
  - name: "imu_available_chr"
    uuid: "19B10002-E8F2-537E-4F6C-D104768A1214"
    data_type: "UnsignedChar"
    properties:
      - "Read"
  - name: "num_pixels_chr"
    uuid: "19B10011-E8F2-537E-4F6C-D104768A1214"
    data_type: "UnsignedChar"
    properties:
      - "Read"
  - name: "brightness_chr"
    uuid: "19B10012-E8F2-537E-4F6C-D104768A1214"
    data_type: "UnsignedChar"
    properties:
      - "Read"
      - "Write"
  - name: "num_colors_chr"
    uuid: "19B10021-E8F2-537E-4F6C-D104768A1214"
    data_type: "UnsignedChar"
    properties:
      - "Read"
      - "Write"
  - name: "color01_chr"
    uuid: "19B10022-E8F2-537E-4F6C-D104768A1214"
    data_type: "UnsignedInt"
    properties:
      - "Read"
      - "Write"
  - name: "color02_chr"
    uuid: "19B10023-E8F2-537E-4F6C-D104768A1214"
    data_type: "UnsignedInt"
    properties:
      - "Read"
      - "Write"
  - name: "color03_chr"
    uuid: "19B10024-E8F2-537E-4F6C-D104768A1214"
    data_type: "UnsignedInt"
    properties:
      - "Read"
      - "Write"
  - name: "color04_chr"
    uuid: "19B10025-E8F2-537E-4F6C-D104768A1214"
    data_type: "UnsignedInt"
    properties:
      - "Read"
      - "Write"
  - name: "blending_chr"
    uuid: "19B10026-E8F2-537E-4F6C-D104768A1214"
    data_type: "UnsignedChar"
    properties:
      - "Read"
      - "Write"
  - name: "fluctuation_chr"
    uuid: "19B10028-E8F2-537E-4F6C-D104768A1214"
    data_type: "UnsignedChar"
    properties:
      - "Read"
      - "Write"
  - name: "transition_chr"
    uuid: "19B10027-E8F2-537E-4F6C-D104768A1214"
    data_type: "UnsignedChar"
    properties:
      - "Read"
      - "Write"
```

## Add XIAO BLE to PlatformIO

Alwin Arrasyid introduces the way to install XIAO BLE board to PlatformIO:  
https://medium.com/@alwint3r/working-with-seeed-xiao-ble-sense-and-platformio-ide-5c4da3ab42a3

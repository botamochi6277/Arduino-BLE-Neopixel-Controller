# XIAO BLE Neopixel Controller

Control Neopixel-strip with [Seeeduino XIAO BLE](https://wiki.seeedstudio.com/XIAO_BLE/)

You can change the lighting colors of the pixels with BLE.

```mermaid
graph TB

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

## Add XIAO BLE to PlatformIO

Alwin Arrasyid introduces the way to install XIAO BLE board to PlatformIO:  
https://medium.com/@alwint3r/working-with-seeed-xiao-ble-sense-and-platformio-ide-5c4da3ab42a3

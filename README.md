# ESPHome VINDRIKTNING Particle Sensor

The IKEA VINDRIKTNING particle sensor is [simple and cheap](https://www.ikea.com/au/en/p/vindriktning-air-quality-sensor-50498243/), costing only $15 AUD. It's easy to attach an ESP device to three test points on the device's PCB to read the communication between the sensor and the IC on the board. 

This project takes the excellent work of Sören Beye in the [Hypfer/esp8266-vindriktning-particle-sensor](https://github.com/Hypfer/esp8266-vindriktning-particle-sensor) project and incorporates it into a custom ESPHome component that can be used as an [External ESPHome component](https://esphome.io/components/external_components.html).


| Device in Home Assistant | PM 2.5 over time |
| --- | --- |
| ![Home Assistant Device](./img/home_assistant1.png) | ![Home Assistant - PM 2.5 values](./img/home_assistant2.png) |

## Rationale

Most of the boilerplate code and configuration of the original project can be abstracted away by ESPHome. Things like Wifi configuration, OTA updates, Home Assistant auto discovery and integration and UART setup can be configured with a YAML file. 

The original project contains a moving average value for the sensor, this can be replicated with a [sliding_window_moving_average](https://esphome.io/components/sensor/index.html#sliding-window-moving-average) filter on the sensor.

## Prerequisites

To attach your ESP8266 to your sensor, follow the excellent build instructions contained within the [original README](https://github.com/Hypfer/esp8266-vindriktning-particle-sensor#readme).

## Example YAML

```yaml
esphome:
    name: vindriktning-pm-1
    platform: ESP8266
    board: d1_mini_pro

external_components:
- source: github://jessedc/esphome-vindriktning-particle-sensor@main
  components: [ vindriktning ]
  refresh: 0s

logger:

api:

ota:
    password: !secret ota_password

wifi:
    ssid: !secret wifi_ssid
    password: !secret wifi_password

uart:
    id: uart_bus
    rx_pin: GPIO4
    tx_pin: GPIO5 # Unused
    baud_rate: 9600

sensor:
- platform: custom
  lambda: |-
    auto vindriktning_pm25_sensor = new VindriktningComponent(id(uart_bus));
    App.register_component(vindriktning_pm25_sensor);
    return {vindriktning_pm25_sensor};
  sensors:
    id: uart_vindrikning_pm25
    name: PM 2.5μm
    unit_of_measurement: μg/m³
    accuracy_decimals: 0
    icon: mdi:air-filter
    state_class: measurement
    device_class: pm25
    filters:
    - sliding_window_moving_average:
        window_size: 5
        send_every: 5
```

## Example Log

```
[11:46:35][D][custom:072]: Received PM 2.5 reading: 19
[11:46:37][D][custom:072]: Received PM 2.5 reading: 19
[11:46:40][D][custom:072]: Received PM 2.5 reading: 19
[11:46:42][D][custom:072]: Received PM 2.5 reading: 19
[11:46:44][D][custom:072]: Received PM 2.5 reading: 19
[11:46:44][D][sensor:125]: 'PM 2.5μm': Sending state 19.00000 μg/m³ with 0 decimals of accuracy
[11:46:46][D][custom:072]: Received PM 2.5 reading: 19
[11:47:07][D][custom:072]: Received PM 2.5 reading: 18
[11:47:08][D][custom:072]: Received PM 2.5 reading: 18
[11:47:11][D][custom:072]: Received PM 2.5 reading: 18
[11:47:13][D][custom:072]: Received PM 2.5 reading: 18
[11:47:13][D][sensor:125]: 'PM 2.5μm': Sending state 18.20000 μg/m³ with 0 decimals of accuracy
```

## Build Photos

I followed the [instructions](https://github.com/Hypfer/esp8266-vindriktning-particle-sensor#readme) to build my own ESP8266 powered Vindriktning sensor, wiring up Gnd, 5V and the single data pin then stuffing it all back into the enclosure.


| PCB with wires | Wemos D1 Mini attached |
| --- | --- |
| ![Build PCB closeup](./img/build1.jpg) | ![Build PCB and ESP8266 D1 Mini](./img/build2.jpg) |




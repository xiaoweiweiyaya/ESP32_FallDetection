| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C6 | ESP32-H2 | ESP32-S2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | -------- | -------- |

# Fall Detection

(See the README.md file in the upper level 'component' directory for more information about examples.)

This is an embedded code for user fall prevention implemented using ESP32-S3, including these components.

alarm mqtt gps led motor mpu6050 oled wifi posture.

The core fall detection algorithm can be found in the "posture" component.


The `espmqtt led_strip` is installed via [component manager](main/idf_component.yml).

## How to Use Example

Before project configuration and build, be sure to set the correct chip target using `idf.py set-target <chip_name>`.

### Hardware Required

* ESP32-S3-DevKitC-1
* 1 x LED
* 1 x LED Strip
* 1 x MPU6050
* 1 x OLED
* 1 x Motor
* 1 x Alarm Module
* 1 x Wifi Module
* 1 x GPS Module

See the [Development Boards](https://www.espressif.com/en/products/devkits) for more information about it.
* A USB cable for Power supply and programming

Some development boards use an addressable LED instead of a regular one. These development boards include:

| Board                | LED type             | Pin                  |
| -------------------- | -------------------- | -------------------- |
| ESP32-C3-DevKitC-1   | Addressable          | GPIO8                |
| ESP32-C3-DevKitM-1   | Addressable          | GPIO8                |
| ESP32-S2-DevKitM-1   | Addressable          | GPIO18               |
| ESP32-S2-Saola-1     | Addressable          | GPIO18               |
| ESP32-S3-DevKitC-1   | Addressable          | GPIO48               |

See [Development Boards](https://www.espressif.com/en/products/devkits) for more information about it.

### Configure the Project

Open the project configuration menu (`idf.py menuconfig`).

In the `Example Configuration` menu:

* Select the LED type in the `Blink LED type` option.
  * Use `GPIO` for regular LED blink.
* Set the GPIO number used for the signal in the `Blink GPIO number` option.
* Set the blinking period in the `Blink period in ms` option.

### Build and Flash

Run `idf.py -p PORT flash monitor` to build, flash and monitor the project.

(To exit the serial monitor, type ``Ctrl-]``.)

See the [Getting Started Guide](https://docs.espressif.com/projects/esp-idf/en/latest/get-started/index.html) for full steps to configure and use ESP-IDF to build projects.

## Example Output

By running this instance, you will see the attitude data and temperature continuously uploaded by the MPU6050, as well as the alarm data sent by the MQTT protocol, which you can change according to the functions in each module to meet your own needs, and if you have any questions, you can send a private message to 1991161087@qq.com

```text
I (9917) MQTT: MQTT_EVENT_PUBLISHED, msg_id=3082
I (10187) POSTURE: acce_x:-0.35, acce_y:0.18, acce_z:-0.96
I (10187) POSTURE: gyro_x:-1.19, gyro_y:-0.08, gyro_z:1.24
I (10197) POSTURE: t:27.68 
I (10197) POSTURE: pitch:-1.66 roll:2.29 
I (10997) POSTURE: acce_x:-0.36, acce_y:0.17, acce_z:-0.96
I (10997) POSTURE: gyro_x:-0.67, gyro_y:0.14, gyro_z:0.29


Note: If you want to use the mqtt data upload function, please change the ssid and password to the ssid and password of your own AP in the wifi component first.


## Troubleshooting

* If the program crashes, please check the wiring of each component to ensure it is correct.

For any technical queries, please open an [issue](https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you soon.

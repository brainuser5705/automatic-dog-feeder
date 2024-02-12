# Electronic Components Used

| Component | Quantity |
| --------- | -------- |
| Arduino Nano IOT 33 (or any samd-based board with Bluetooth and WiFi module) |`1 |
| hobbyist Servo motors | 2 |
| 4-pin push button | 1 |
| LED | 1 |

# Setup

## Libraries To Installed
- ArduinoBLE
- Servo
- RTCZero
- NTPClient
- WifiNINA

## WiFi Setup

Since the code connects to WiFi to update the internal RTC with the time from an NTP server, create a file named `secret.h` with the following contents:
```c
#ifndef SECRET_H
#define SECRET_H

const char *SSID = /* your wifi name as a string*/
const char *PASS = /* your wifi password as a string */

#endif
```

# Final Product and Usage

Set the variables in `configurations.h` to adjust the functionality of the door, such as delay time and automatic activation schedule.

Read more about usage [here](https://ashleyliew.com/current/small-arduino-programs.html).
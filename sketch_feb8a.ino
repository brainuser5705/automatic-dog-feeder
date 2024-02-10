#include <ArduinoBLE.h>
#include <Servo.h>
#include <RTCZero.h>
#include <NTPClient.h>
#include <WiFiNINA.h>
#include <WiFiUdp.h>

/* Contains configuration variables for controlling door */
#include "configurations.h"
/* Contains WiFi credentials */
#include "secret.h"

/* Servo objects that control the door */
Servo servoL, servoR;
/* Button to activate door set to digital pin 9 (interrupt pin) */
const int button = 9;
/* Led to signal door sequence set to digital pin 5 */
const int led = 5;
/* Boolean for button to trigger door sequence */
bool buttonActivate = false;

// Bluetooth service for peripheral device (Arduino)
BLEService doorControlService("180A");
// remote device can read/write to characteristic
BLEByteCharacteristic servoCharacteristic("2A57", BLERead | BLEWrite);

/* Using internal RTC of microcontroller to set automatic door activation */
RTCZero alarm;
WiFiUDP udpSocket;
NTPClient timeClient(udpSocket, "us.pool.ntp.org", -18000); // offset of 5 hours (UTC-5)
/* Intialized string needed for string addition when printing out time */
String colon = ":";
/* Boolean for RTC alarm to trigger door sequence */
bool alarmActivate = false;

void setup() {
  Serial.begin(9600);

  WiFi.begin(SSID, PASS);
  if ( WiFi.status() != WL_CONNECTED ){
    Serial.println("Wifi cannot connect...");
    while (1);
  }
  Serial.println("Wifi connected");

  // initalizes RTCZero lib
  alarm.begin(); 
  // updates alarm time from NTP server
  timeClient.begin();
  timeClient.update();
  int hour = timeClient.getHours(), minute = timeClient.getMinutes(), second = timeClient.getSeconds();
  Serial.print("Updating RTC to ");
  Serial.println(hour + colon + minute + colon + second);
  alarm.setTime(hour, minute, second);
  // schedule time for automatic door activation
  adjustAlarmTime();
  alarm.enableAlarm(alarm.MATCH_HHMMSS);
  alarm.attachInterrupt(toggleAlarmActivate);

  // attached to PWM pin
  servoR.attach(2);
  servoL.attach(3);
  // set to initial closed position
  servoR.write(MAX_POS);
  servoL.write(MAX_POS);
  // button code linked to interrupt
  pinMode(button, INPUT_PULLDOWN); // don't need pulldown resistor on the circuit
  attachInterrupt(digitalPinToInterrupt(button), toggleButtonActivate, FALLING);
  pinMode(led, OUTPUT);

  /* Following code is taken from ArduinoBLE library example code */

  // begin initialization
  if (!BLE.begin()){
    Serial.println("Starting Bluetooth® Low Energy module failed!");
    while (1);
  }

  // set local name peripheral advertises
  BLE.setLocalName("Dog Feeder");
  // set up UUID for the service peripheral advertises
  BLE.setAdvertisedService(doorControlService);
  // add characteristics to service
  doorControlService.addCharacteristic(servoCharacteristic);
  // add service
  BLE.addService(doorControlService);
  // set initial value
  servoCharacteristic.writeValue(0);
  // start advertising
  BLE.advertise();

  Serial.println("Bluetooth® device active and ready for connection");
  
}


void loop() {
  // poll for Bluetooth events
  BLE.poll();

  // triggers are physical button press or Bluetooth write
  if (buttonActivate || alarmActivate || (servoCharacteristic.written() && servoCharacteristic.value())){
    doorSequence();
    if (buttonActivate) toggleButtonActivate(); // only necessary if physical button was pressed - reset to close
    else if (alarmActivate) toggleAlarmActivate(); // resets to false
  }
}


void toggleButtonActivate(){
  buttonActivate = !buttonActivate;
}

void toggleAlarmActivate(){
  alarmActivate = !alarmActivate;
  if (alarmActivate) adjustAlarmTime(); // only adjust time when triggered by RTC
}

void doorSequence(){
  openSesame();
  delay(DELAY * 1000);
  closeSesame();
}

/*
  Opens the door
*/
void openSesame(){
  digitalWrite(led, HIGH);
  Serial.println("Opening door...");
  for (int pos = MAX_POS; pos > MIN_POS; pos--){
    servoR.write(MAX_POS - pos);
    servoL.write(pos);
    delay(SPEED);
  }
}

/*
  Closes the door
*/
void closeSesame(){
  digitalWrite(led, LOW);
  Serial.println("Closing door...");
  for (int pos = MIN_POS; pos < MAX_POS; pos++){
    servoR.write(MAX_POS - pos);
    servoL.write(pos);
    delay(SPEED);
  }
}

/*
  Changes alarm time to next sequential time in
  ALARM_TIMES
*/
void adjustAlarmTime(){
  static int i = 0;
  const int* time = ALARM_TIMES[i];
  alarm.setAlarmTime(time[0], time[1], time[2]);
  Serial.print("Next activation time set to: ");
  Serial.println(time[0] + colon + time[1] + colon + time[2]);
  i = (i+1) % ALARM_TIMES_SIZE; // cycles through the indices
}

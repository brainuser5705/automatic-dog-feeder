#include <ArduinoBLE.h>
#include <Servo.h>

const int MAX_POS = 150;
const int SPEED = 15; // smaller val -> faster

Servo servoL, servoR; // servos controlling door
const int button = 9; // button to activate door set to digital pin 9 (interrupt pin)
const int led = 5;    // led to signal door sequence
bool buttonPressed = false;

// Bluetooth service for peripheral device (Arduino)
BLEService doorControlService("180A");
// remote device can read/write to characteristic
BLEByteCharacteristic servoCharacteristic("2A57", BLERead | BLEWrite);

void setup() {
  Serial.begin(9600);
  while (!Serial);

  // attached to PWM pin
  servoR.attach(2);
  servoL.attach(3);
  // set to initial closed position
  servoR.write(MAX_POS);
  servoL.write(MAX_POS);

  pinMode(button, INPUT_PULLDOWN); // don't need pullup resistor on the circuit
  attachInterrupt(digitalPinToInterrupt(button), toggleButtonPressed, FALLING);
  pinMode(led, OUTPUT);

  /* Following code is taken from ArduinoBLE library example code */

  // begin initialization
  if (!BLE.begin()){
    Serial.println("tarting Bluetooth® Low Energy module failed!");
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

  Serial.println("Bluetooth® device active, waiting for connections...");
  
}


void loop() {
  // listen for Bluetooth® Low Energy peripherals to connect:
  BLEDevice central = BLE.central();

  // if a central is connected to peripheral:
  if (central) {
    Serial.print("Connected to central: ");
    // print the central's MAC address:
    Serial.println(central.address());

    // while the central is still connected to peripheral:
    while (central.connected()) {
      if ((servoCharacteristic.written() && servoCharacteristic.value()) || buttonPressed){
        doorSequence();
      }
    }

    Serial.print(F("Disconnected from central: "));
    Serial.println(central.address());

  }

}

void toggleButtonPressed(){
  buttonPressed = !buttonPressed;
  Serial.print("button is now: ");
  Serial.println(buttonPressed);
}

void doorSequence(){
  openSesame();
  delay(2000);
  closeSesame();
  if (buttonPressed) toggleButtonPressed();
}


/**
  Opens the door
*/
void openSesame(){
  digitalWrite(led, HIGH);
  Serial.println("Opening door...");
  for (int pos = 150; pos > 0; pos--){
    servoR.write(MAX_POS - pos);
    servoL.write(pos);
    delay(SPEED);
  }
}

/**
  Closes the door
*/
void closeSesame(){
  digitalWrite(led, LOW);
  Serial.println("Closing door...");
  for (int pos = 0; pos < MAX_POS; pos++){
    servoR.write(MAX_POS - pos);
    servoL.write(pos);
    delay(SPEED);
  }
}

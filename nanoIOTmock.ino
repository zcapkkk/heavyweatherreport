#include <ArduinoBLE.h>
#include "Adafruit_Sensor.h"
#include "Adafruit_AM2320.h"

Adafruit_AM2320 am2320 = Adafruit_AM2320();

// Demo mockup sketch for weather comparison
// Based on provided LED Sketch
BLEService ledService("19B10000-E8F2-537E-4F6C-D104768A1214"); // BLE LED Service
BLEUnsignedCharCharacteristic readings("2A19",  // standard 16-bit characteristic UUID
    BLERead | BLENotify); // remote clients will be able to get notifications if this characteristic changes

BLEService weatherService("130b416b-45ef-4698-87ef-b8fe6f50cd3d"); // Weather Service
BLEFloatCharacteristic temp("4A20", BLERead | BLENotify);
BLEFloatCharacteristic humd("4A21", BLERead | BLENotify);

// BLE LED Switch Characteristic - custom 128-bit UUID, read and writable by central
BLEByteCharacteristic switchCharacteristic("19B10001-E8F2-537E-4F6C-D104768A1214", BLERead | BLEWrite);

const int ledPin = LED_BUILTIN; // pin to use for the LED
int limit = 20; // set value to switch on when pass limit
long oldtime;
long currtime;
float tempVal;
float humdVal;

void setup() {
  Serial.begin(9600);
  while (!Serial);

  // set LED pin to output mode
  pinMode(ledPin, OUTPUT);

  // begin initialization
  if (!BLE.begin()) 
  {
    Serial.println("starting BLE failed!");

    while (1);
  }

  // set advertised local name and service UUID:
  BLE.setLocalName("Weather Station");
  BLE.setAdvertisedService(ledService);

  // add the characteristic to the service
  ledService.addCharacteristic(switchCharacteristic);

  // add service
  BLE.addService(ledService);

  // set the initial value for the characeristic:
  switchCharacteristic.writeValue(0);

  // start advertising
  BLE.advertise();

  Serial.println("BLE LED Peripheral");


  // setup weather report

  weatherService.addCharacteristic(temp);
  weatherService.addCharacteristic(humd);

  BLE.addService(weatherService);

  am2320.begin();

  oldtime = millis();
}

void loop() {
  // listen for BLE peripherals to connect:
  BLEDevice central = BLE.central();

  // if a central is connected to peripheral:
  if (central) {
    Serial.print("Connected to central: ");
    // print the central's MAC address:
    Serial.println(central.address());

    // while the central is still connected to peripheral:
    while (central.connected()) {
      // if the remote device wrote to the characteristic,
      // use the value to control the LED:
      if (switchCharacteristic.written() ) {
        if (switchCharacteristic.value()> limit) {   // value bigger than limit
          Serial.println("Too hot, turning on");
          digitalWrite(ledPin, HIGH);         // will turn the LED on
        } else {                              // a 0 value
          Serial.println(F("Cool enough, turning off"));
          digitalWrite(ledPin, LOW);          // will turn the LED off
        }
      }
      
     currtime = millis();

    if (currtime - oldtime >= 1000)
    {
      oldtime = currtime;
      
      tempVal =  am2320.readTemperature();
      humdVal =  am2320.readHumidity();

      temp.writeValue(tempVal);
      humd.writeValue(humdVal);

      Serial.println("Temperature is: " + (String) tempVal + " Humidity is: " + (String) humdVal);

    }
    }

    
    

    // when the central disconnects, print it out:
    Serial.print(F("Disconnected from central: "));
    Serial.println(central.address());
  }
}

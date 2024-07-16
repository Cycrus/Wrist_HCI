#include <ArduinoBLE.h>

BLEService timeService("1805"); // UUID for Current Time Service
BLEUnsignedLongCharacteristic currentTimeChar("2A2B", BLEWrite); // UUID for Current Time Characteristic

void setup() {
  Serial.begin(9600);
  while (!Serial);

  if (!BLE.begin()) {
    Serial.println("Starting BLE failed!");
    while (1);
  }

  BLE.setLocalName("Nano33BLE_Time");
  BLE.setAdvertisedService(timeService);
  timeService.addCharacteristic(currentTimeChar);
  BLE.addService(timeService);

  currentTimeChar.writeValue(0); // Initialize characteristic with default value

  BLE.advertise();
  Serial.println("Bluetooth device active, waiting for connections...");
}

void loop() {
  BLEDevice central = BLE.central();

  if (central) {
    Serial.print("Connected to central: ");
    Serial.println(central.address());

    while (central.connected()) {
      if (currentTimeChar.written()) {
        unsigned long currentTime = currentTimeChar.value();
        Serial.print("Received time: ");
        Serial.println(currentTime);
      }
    }

    Serial.print("Disconnected from central: ");
    Serial.println(central.address());
  }
}

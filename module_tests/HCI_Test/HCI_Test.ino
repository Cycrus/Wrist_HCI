#include <ArduinoBLE.h>

#define MOD_NONE 0x00
#define MOD_LEFT_CTR 0x01
#define MOD_LEFT_SHIFT 0x02
#define MOD_LEFT_ALT 0x04
#define MOD_LEFT_GUI 0x08
#define MOD_RIGHT_CTR 0x10
#define MOD_RIGHT_SHIFT 0x20
#define MOD_RIGHT_ALT 0x40
#define MOD_RIGHT_GUI 0x80

#define MOUSE_LEFT 0x01
#define MOUSE_RIGHT 0x02
#define MOUSE_MIDDLE 0x04

#define ICON_GENERIC 0x03C0
#define ICON_KEYBOARD 0x03C1
#define ICON_MOUSE 0x03C2

// HID report descriptor
const uint8_t HID_REPORT_DESCRIPTOR[] = {
  // Keyboard
  0x05, 0x01,        // Usage Page (Generic Desktop)
  0x09, 0x06,        // Usage (Keyboard)
  0xA1, 0x01,        // Collection (Application)
  0x85, 0x01,        // Report ID (1)
  0x05, 0x07,        // Usage Page (Key Codes)
  0x19, 0xE0,        // Usage Minimum (224)
  0x29, 0xE7,        // Usage Maximum (231)
  0x15, 0x00,        // Logical Minimum (0)
  0x25, 0x01,        // Logical Maximum (1)
  0x75, 0x01,        // Report Size (1)
  0x95, 0x08,        // Report Count (8)
  0x81, 0x02,        // Input (Data, Variable, Absolute)
  0x95, 0x01,        // Report Count (1)
  0x75, 0x08,        // Report Size (8)
  0x81, 0x03,        // Input (Constant) reserved byte(1)
  0x95, 0x05,        // Report Count (5)
  0x75, 0x01,        // Report Size (1)
  0x05, 0x08,        // Usage Page (Page# for LEDs)
  0x19, 0x01,        // Usage Minimum (1)
  0x29, 0x05,        // Usage Maximum (5)
  0x91, 0x02,        // Output (Data, Variable, Absolute), Led report
  0x95, 0x01,        // Report Count (1)
  0x75, 0x03,        // Report Size (3)
  0x91, 0x03,        // Output (Constant), Led report padding
  0x95, 0x06,        // Report Count (6)
  0x75, 0x08,        // Report Size (8)
  0x15, 0x00,        // Logical Minimum (0)
  0x25, 0x65,        // Logical Maximum (101)
  0x05, 0x07,        // Usage Page (Key codes)
  0x19, 0x00,        // Usage Minimum (0)
  0x29, 0x65,        // Usage Maximum (101)
  0x81, 0x00,        // Input (Data, Array) Key array(6 bytes)
  0xC0,              // End Collection (Application)

  // Mouse
  0x05, 0x01,        // Usage Page (Generic Desktop)
  0x09, 0x02,        // Usage (Mouse)
  0xA1, 0x01,        // Collection (Application)
  0x85, 0x02,        //   Report ID (2)
  0x09, 0x01,        //   Usage (Pointer)
  0xA1, 0x00,        //   Collection (Physical)
  0x05, 0x09,        //     Usage Page (Button)
  0x19, 0x01,        //     Usage Minimum (Button 1)
  0x29, 0x03,        //     Usage Maximum (Button 3)
  0x15, 0x00,        //     Logical Minimum (0)
  0x25, 0x01,        //     Logical Maximum (1)
  0x95, 0x03,        //     Report Count (3)
  0x75, 0x01,        //     Report Size (1)
  0x81, 0x02,        //     Input (Data, Variable, Absolute)
  0x95, 0x01,        //     Report Count (1)
  0x75, 0x05,        //     Report Size (5)
  0x81, 0x03,        //     Input (Const, Variable, Absolute)
  0x05, 0x01,        //     Usage Page (Generic Desktop)
  0x09, 0x30,        //     Usage (X)
  0x09, 0x31,        //     Usage (Y)
  0x09, 0x38,        //     Usage (Wheel)
  0x15, 0x81,        //     Logical Minimum (-127)
  0x25, 0x7F,        //     Logical Maximum (127)
  0x75, 0x08,        //     Report Size (8)
  0x95, 0x03,        //     Report Count (3)
  0x81, 0x06,        //     Input (Data, Variable, Relative)
  0xC0,              //   End Collection (Physical)
  0xC0               // End Collection (Application)
};

BLEService hid_service("1812"); // HID service
BLECharacteristic hid_report_map("2A4B", BLERead, sizeof(HID_REPORT_DESCRIPTOR), true);
BLECharacteristic hid_control_point("2A4C", BLEWriteWithoutResponse, 1, true);
BLECharacteristic keyboard_report("2A4D", BLERead | BLENotify, 9, true); // Keyboard report
BLECharacteristic mouse_report("2A4D", BLERead | BLENotify, 5, true); // Mouse report


void setup()
{
  Serial.begin(9600);
  while (!Serial);

  if (!BLE.begin())
  {
    Serial.println("starting BLE failed!");
    while (1);
  }

  BLE.setLocalName("Arduino HID");
  BLE.setAdvertisedService(hid_service);

  hid_service.addCharacteristic(hid_report_map);
  hid_service.addCharacteristic(hid_control_point);
  hid_service.addCharacteristic(keyboard_report);
  hid_service.addCharacteristic(mouse_report);

  BLE.addService(hid_service);

  hid_report_map.writeValue(HID_REPORT_DESCRIPTOR, sizeof(HID_REPORT_DESCRIPTOR));
  hid_control_point.writeValue((uint8_t)0x00);

  BLE.setAppearance(ICON_GENERIC);

  BLE.advertise();

  Serial.println("Bluetooth device active, waiting for connections...");
}

void loop()
{
  BLEDevice central = BLE.central();

  if (central)
  {
    Serial.print("Connected to central: ");
    Serial.println(central.address());

    while (central.connected())
    {
      // Example usage
      Serial.println("A pressed.");
      sendKeyboardPress('a', MOD_NONE);
      delay(100);
      sendKeyboardRelease();
      delay(1000);
      Serial.println("Mouse moved.");
      sendMouseMove(20, 20);
      delay(1000);
    }

    Serial.print("Disconnected from central: ");
    Serial.println(central.address());
  }
}

void sendKeyboardPress(char key, uint8_t modifier)
{
  /* Format:
    0. BLE Service ID
    1. Modifier
    2. Reserved
    3. Key 1
    4. Key 2
    5. Key 3
    6. Key 4
    7. Key 5
    8. Key 6
   */
  uint8_t key_report_message[9] = {0x01, modifier, 0, key - 93, 0, 0, 0, 0, 0};
  keyboard_report.writeValue(key_report_message, sizeof(key_report_message));
}

void sendKeyboardRelease()
{
  uint8_t key_report_message[9] = {0x01, 0, 0, 0, 0, 0, 0, 0, 0}; // All zeroes to indicate no key pressed
  keyboard_report.writeValue(key_report_message, sizeof(key_report_message));
}

void sendMouseMove(int8_t x, int8_t y)
{
  uint8_t mouse_report_message[5] = {0x02, 0x00, (uint8_t)x, (uint8_t)y, 0x00}; // Report ID 2, no button pressed, x movement, y movement
  mouse_report.writeValue(mouse_report_message, sizeof(mouse_report_message));
}

void sendMousePress(uint8_t button)
{
  uint8_t mouse_report_message[5] = {0x02, button, 0x00, 0x00, 0x00};
  mouse_report.writeValue(mouse_report_message, sizeof(mouse_report_message));
}

void sendMouseRelease()
{
  uint8_t mouse_report_message[5] = {0x02, 0x00, 0x00, 0x00, 0x00};
  mouse_report.writeValue(mouse_report_message, sizeof(mouse_report_message));
}

void sendMouseScroll(int8_t wheel) {
  uint8_t mouse_report_message[5] = {0x02, 0x00, 0, 0, wheel}; // Report ID 2, no button pressed, no movement, wheel movement
  mouse_report.writeValue(mouse_report_message, sizeof(mouse_report_message));
}
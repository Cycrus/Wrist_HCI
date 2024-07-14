/**********************************************************************
 * BLE_HID.hpp
 * 
 * A class to advertise a bluetooth keyboard/mouse combo.
 * 
 * Author: Cyril Marx
 * Created: July 2024
 **********************************************************************/

#ifndef BLE_HID_HPP
#define BLE_HID_HPP

#include <Arduino.h>
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

#define MOUSE_SCROLL_UP 1
#define MOUSE_SCROLL_DOWN -1

#define ICON_GENERIC 0x03C0
#define ICON_KEYBOARD 0x03C1
#define ICON_MOUSE 0x03C2

#define KEYBOARD_FIELD_MODIFIER 1
#define KEYBOARD_FIELD_BUTTON 3

#define MOUSE_FIELD_BUTTON 1
#define MOUSE_FIELD_X 2
#define MOUSE_FIELD_Y 3
#define MOUSE_FIELD_WHEEL 4

#define KEYBOARD_MESSAGE_LEN 9
#define MOUSE_MESSAGE_LEN 5

#define KEYBOARD_ID 0x01
#define MOUSE_ID 0x02

const uint8_t HID_REPORT_DESCRIPTOR[] = {
  // Keyboard
  0x05, 0x01,        // Usage Page (Generic Desktop)
  0x09, 0x06,        // Usage (Keyboard)
  0xA1, 0x01,        // Collection (Application)
  0x85, KEYBOARD_ID,        // Report ID (1)
  0x05, 0x07,        // Usage Page (Key Codes)
  0x19, 0xE0,        // Usage Minimum (224)
  0x29, 0xE7,        // Usage Maximum (231)
  0x15, 0x00,        // Logical Minimum (0)
  0x25, 0x01,        // Logical Maximum (1)
  0x75, 0x01,        // Report Size (1)
  0x95, 0x08,        // Report Count (8)
  0x81, 0x02,        // Input (Data, Variable, Absolute)
  0x95, 0x01,        // Report Count (1)
  0x75, KEYBOARD_MESSAGE_LEN,        // Report Size (9)
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
  0x85, MOUSE_ID,        //   Report ID (2)
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
  0x75, MOUSE_MESSAGE_LEN,        //     Report Size (5)
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

class BLE_HID
{
    public:
        BLE_HID();
        void initService(const char* device_name);

        bool checkRemoteAvailability();
        bool checkRemoteConnection();
        
        void setKeyboardButtonPress(char button, uint8_t modifier);
        void setMouseButtonPress(uint8_t button);
        void setMouseScroll(int8_t wheel);
        void setMouseMove(int8_t x, int8_t y);

        void resetKeyboardMessage();
        void resetMouseMessage();

        void sendKeyboardRelease();
        void sendMouseRelease();

        void sendKeyboardMessage();
        void sendMouseMessage();

    private:
        BLEService _hid_service;
        BLECharacteristic _hid_report_map;
        BLECharacteristic _hid_control_point;
        BLECharacteristic _keyboard_report;
        BLECharacteristic _mouse_report;
        BLEDevice _remote_device;

        uint8_t _key_report_message[KEYBOARD_MESSAGE_LEN];
        uint8_t _mouse_report_message[MOUSE_MESSAGE_LEN];

};

#endif // BLE_HID_HPP
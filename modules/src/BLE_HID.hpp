/**********************************************************************
* BLE_HID.hpp
* 
* A class to advertise a device as a bluetooth keyboard/mouse combo.
* Allows to fully define a keyboard and a mouse report separately and
* sending out the full reports at a later time.
* Workflow:
*   1. Check for connection
*   2. Define messages with functions upon desired events
*   3. Send out report messages with dedicated functions
*      (reports are cleared afterwards automatically)
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

#define MAX_KEYBOARD_KEYS 6

//The descriptor for the human interface device. Needed to format messages, and
// how the host device should interpret the incoming messages.
//
const uint8_t HID_REPORT_DESCRIPTOR[] = {
    // Keyboard
    0x05, 0x01,        // Usage Page (Generic Desktop)
    0x09, 0x06,        // Usage (Keyboard)
    0xA1, 0x01,        // Collection (Application)
    0x85, KEYBOARD_ID, // Report ID (1)
    0x05, 0x07,        // Usage Page (Key Codes)
    0x19, 0xE0,        // Usage Minimum (224)
    0x29, 0xE7,        // Usage Maximum (231)
    0x15, 0x00,        // Logical Minimum (0)
    0x25, 0x01,        // Logical Maximum (1)
    0x75, 0x01,        // Report Size (1)
    0x95, 0x08,        // Report Count (8)
    0x81, 0x02,        // Input (Data, Variable, Absolute)
    0x95, 0x01,        // Report Count (1)
    0x75, (KEYBOARD_MESSAGE_LEN - 1),        // Report Size (8)
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
    0x85, MOUSE_ID,    // Report ID (2)
    0x09, 0x01,        // Usage (Pointer)
    0xA1, 0x00,        // Collection (Physical)
    0x05, 0x09,        // Usage Page (Button)
    0x19, 0x01,        // Usage Minimum (Button 1)
    0x29, 0x03,        // Usage Maximum (Button 3)
    0x15, 0x00,        // Logical Minimum (0)
    0x25, 0x01,        // Logical Maximum (1)
    0x95, 0x03,        // Report Count (3)
    0x75, 0x01,        // Report Size (1)
    0x81, 0x02,        // Input (Data, Variable, Absolute)
    0x95, 0x01,        // Report Count (1)
    0x75, (MOUSE_MESSAGE_LEN - 1),        // Report Size (5)
    0x81, 0x03,        // Input (Const, Variable, Absolute)
    0x05, 0x01,        // Usage Page (Generic Desktop)
    0x09, 0x30,        // Usage (X)
    0x09, 0x31,        // Usage (Y)
    0x09, 0x38,        // Usage (Wheel)
    0x15, 0x81,        // Logical Minimum (-127)
    0x25, 0x7F,        // Logical Maximum (127)
    0x75, 0x08,        // Report Size (8)
    0x95, 0x03,        // Report Count (3)
    0x81, 0x06,        // Input (Data, Variable, Relative)
    0xC0,              // End Collection (Physical)
    0xC0               // End Collection (Application)
};

class BLE_HID
{
    public:
    //-----------------------------------------------------------------------------------------------------------------
    ///
    /// Constructor. Defines the UUIDs and the hid subsystems.
    //
    BLE_HID();

    //-----------------------------------------------------------------------------------------------------------------
    ///
    /// Initializes the BLE service. Sets the device name, setups all subsystems and advertises the service
    /// at the end.
    ///
    /// @param  device_name         The name the device should be advertised as.
    //
    void initService(const char* device_name);

    //-----------------------------------------------------------------------------------------------------------------
    ///
    /// Checks if a remote device exists yet.
    ///
    /// @return True if a remote device exists.
    //
    bool checkRemoteAvailability();

    //-----------------------------------------------------------------------------------------------------------------
    ///
    /// Checks if a remote device is connected. Needs to be checked regularly in the main loop to allow a connection to
    /// happen.
    ///
    /// @return True if a remote device is connected.
    //
    bool checkRemoteConnection();

    //-----------------------------------------------------------------------------------------------------------------
    ///
    /// Adds a new keyboard button to press to the buffer. If called consequtively it can store up to 6 buttons. Afterwards
    /// the call is ignored. Either sendKeyboardMessage() or resetKeyboardMessage() need to be called to allow for new buttons
    /// to be pressed again.
    /// Does not yet send the instruction to the remote device. Call sendKeyboardMessage() afterwards to do so.
    ///
    /// @param button       The button which should be pressed as a char.
    /// @param modifier     The modifier which is used (alt, ctr, shift). If multiple should be used, just or them together
    ///                     (see macros above).
    //
    void setKeyboardButtonPress(char button, uint8_t modifier);

    //-----------------------------------------------------------------------------------------------------------------
    ///
    /// Adds a mouse button to be pressed to the buffer.
    /// Does not yet send the instruction to the remote device. Call sendMouseMessage() afterwards to do so.
    ///
    /// @param button       The button which should be pressed as a char (see macros above).
    //
    void setMouseButtonPress(uint8_t button);

    //-----------------------------------------------------------------------------------------------------------------
    ///
    /// Adds a mouse wheel command (see macros above) to the buffer. Can scroll up or down depending on the sign of the wheel
    /// parameter.
    /// Does not yet send the instruction to the remote device. Call sendMouseMessage() afterwards to do so.
    ///
    /// @param wheel       The strength and direction of the scroll wheel.
    //
    void setMouseScroll(int8_t wheel);

    //-----------------------------------------------------------------------------------------------------------------
    ///
    /// Adds a mouse move command to the buffer.
    /// Does not yet send the instruction to the remote device. Call sendMouseMessage() afterwards to do so.
    ///
    /// @param x       The movement strenght on the x axis in pixels.
    /// @param y       The movement strenght on the y axis in pixels.
    //
    void setMouseMove(int8_t x, int8_t y);

    //-----------------------------------------------------------------------------------------------------------------
    ///
    /// Resets the keyboard message buffer and clears all commands out of it.
    /// Does not yet send the instruction to the remote device. Call sendKeyboardMessage() afterwards to do so.
    //
    void resetKeyboardMessage();

    //-----------------------------------------------------------------------------------------------------------------
    ///
    /// Resets the mouse message buffer and clears all commands out of it.
    /// Does not yet send the instruction to the remote device. Call sendMouseMessage() afterwards to do so.
    //
    void resetMouseMessage();

    //-----------------------------------------------------------------------------------------------------------------
    ///
    /// Sends a keyboard release command consisting of only zeros. Can be used to force a key release command without
    /// clearing the message buffer.
    //
    void sendKeyboardRelease();

    //-----------------------------------------------------------------------------------------------------------------
    ///
    /// Sends a mouse button release command consisting of only zeros. Can be used to force a button release command without
    /// clearing the message buffer.
    //
    void sendMouseRelease();

    //-----------------------------------------------------------------------------------------------------------------
    ///
    /// Sends the previously defined keyboard message buffer to the remote device. Automatically clears the buffer afterwards.
    //
    void sendKeyboardMessage();

    //-----------------------------------------------------------------------------------------------------------------
    ///
    /// Sends the previously defined mouse message buffer to the remote device. Automatically clears the buffer afterwards.
    //
    void sendMouseMessage();

    //-----------------------------------------------------------------------------------------------------------------
    ///
    /// A test routine to quickly check if the system works in general. Captures the whole main loop of the program.
    /// Send commands via Serial to the Arduino to trigger certain messages:
    ///
    /// 'a' -> Press 'a' on the keyboard
    /// 'u' -> Move mouse cursor 20 pixels up
    /// 'm' -> Left click mouse
    /// 'w' -> Scroll down once
    //
    void testRoutine();

    private:
    BLEService _hid_service;
    BLECharacteristic _hid_report_map;
    BLECharacteristic _hid_control_point;
    BLECharacteristic _keyboard_report;
    BLECharacteristic _mouse_report;
    BLEDevice _remote_device;

    uint8_t _curr_keyboard_button;

    uint8_t _key_report_message[KEYBOARD_MESSAGE_LEN];
    uint8_t _mouse_report_message[MOUSE_MESSAGE_LEN];

    void __debugPrintMessage(const char* name, uint8_t message[], uint8_t size);
};

#endif // BLE_HID_HPP
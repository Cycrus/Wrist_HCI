/**********************************************************************
 * BLE_HID.cpp
 * 
 * Implementation of the BLE_HID class.
 * 
 * Author: Cyril Marx
 * Created: July 2024
 **********************************************************************/

#include "BLE_HID.hpp"

BLE_HID::BLE_HID() :
    _hid_service("1812"), // HID service
    _hid_report_map("2A4B", BLERead, sizeof(HID_REPORT_DESCRIPTOR), true),
    _hid_control_point("2A4C", BLEWriteWithoutResponse, 1, true),
    _keyboard_report("2A4D", BLERead | BLENotify, KEYBOARD_MESSAGE_LEN, true),
    _mouse_report("2A4D", BLERead | BLENotify, MOUSE_MESSAGE_LEN, true),
    _key_report_message({0x01, 0, 0, 0, 0, 0, 0, 0, 0}),
    _mouse_report_message({0x02, 0, 0, 0, 0})
{ }

void BLE_HID::initService(const char* device_name)
{
    if (!BLE.begin())
    {
        Serial.println("starting BLE failed!");
        while (1);
    }

    Serial.print("Setting up bluetooth device <");
    Serial.print(device_name);
    Serial.println(">.");
    BLE.setLocalName(device_name);
    BLE.setAdvertisedService(_hid_service);
    BLE.setAppearance(ICON_GENERIC);

    _hid_service.addCharacteristic(_hid_report_map);
    _hid_service.addCharacteristic(_hid_control_point);
    _hid_service.addCharacteristic(_keyboard_report);
    _hid_service.addCharacteristic(_mouse_report);

    BLE.addService(_hid_service);

    _hid_report_map.writeValue(HID_REPORT_DESCRIPTOR, sizeof(HID_REPORT_DESCRIPTOR));
    _hid_control_point.writeValue((uint8_t)0x00);

    BLE.advertise();

    Serial.println("Bluetooth device active, waiting for connections...");
}

bool BLE_HID::checkRemoteAvailability()
{
    _remote_device = BLE.central();
    if(_remote_device)
    {
        Serial.print("Found device: ");
        Serial.println(_remote_device.address());
        return true;
    }
    return false;
}

bool BLE_HID::checkRemoteConnection()
{
    if (_remote_device)
    {
        return _remote_device.connected();
    }
    return false;
}

void BLE_HID::setKeyboardButtonPress(char button, uint8_t modifier)
{
    _key_report_message[KEYBOARD_FIELD_MODIFIER] = modifier;
    _key_report_message[KEYBOARD_FIELD_BUTTON] = button - 93;
}

void BLE_HID::setMouseButtonPress(uint8_t button)
{
    _mouse_report_message[MOUSE_FIELD_BUTTON] = button;
}

void BLE_HID::setMouseScroll(int8_t wheel)
{
    _mouse_report_message[MOUSE_FIELD_WHEEL] = wheel;
}

void BLE_HID::setMouseMove(int8_t x, int8_t y)
{
    _mouse_report_message[MOUSE_FIELD_X] = x;
    _mouse_report_message[MOUSE_FIELD_Y] = y;
}

void BLE_HID::resetKeyboardMessage()
{
    for(int i = 1; i < KEYBOARD_MESSAGE_LEN; i++)
        _key_report_message[i] = 0;
    _key_report_message[0] = KEYBOARD_ID;
}

void BLE_HID::resetMouseMessage()
{
    for(int i = 1; i < MOUSE_MESSAGE_LEN; i++)
        _mouse_report_message[i] = 0;
    _mouse_report_message[0] = MOUSE_ID;
}

void BLE_HID::sendKeyboardRelease()
{
    uint8_t release_key_report_message[KEYBOARD_MESSAGE_LEN] = {KEYBOARD_ID, 0, 0, 0, 0, 0, 0, 0, 0};
    _keyboard_report.writeValue(release_key_report_message, sizeof(release_key_report_message));
}

void BLE_HID::sendMouseRelease()
{
    uint8_t release_mouse_report_message[MOUSE_MESSAGE_LEN] = {MOUSE_ID, 0, 0, 0, 0};
    _mouse_report.writeValue(release_mouse_report_message, sizeof(release_mouse_report_message));
}

void BLE_HID::sendKeyboardMessage()
{
    _keyboard_report.writeValue(_key_report_message, sizeof(_key_report_message));
    resetKeyboardMessage();
}

void BLE_HID::sendMouseMessage()
{
    _mouse_report.writeValue(_mouse_report_message, sizeof(_mouse_report_message));
    resetMouseMessage();
}

void BLE_HID::testRoutine()
{
    if(checkRemoteAvailability())
    {
        while(checkRemoteConnection())
        {
            while(Serial.available())
            {
                char debug_input = Serial.read();
                if(debug_input == 'a')
                {
                    Serial.println("Pressing A.");
                    setKeyboardButtonPress('a', MOD_NONE);
                }
                if(debug_input == 'm')
                {
                    Serial.println("Clicking left.");
                    setMouseButtonPress(MOUSE_LEFT);
                }
                if(debug_input == 'u')
                {
                    Serial.println("Moving Mouse Up.");
                    setMouseMove(0, -40);
                }
                if(debug_input == 'w')
                {
                    Serial.println("Scrolling Down.");
                    setMouseScroll(MOUSE_SCROLL_DOWN);
                }
            }

            sendKeyboardMessage();
            sendMouseMessage();
            delay(100);
        }
    }
}

void BLE_HID::__debugPrintMessage(const char* name, uint8_t message[], uint8_t size)
{
    Serial.print("Message <");
    Serial.print(name);
    Serial.print(">: ");
    for(int i = 0; i < size; i++)
    {
        Serial.print(message[i]);
        if(i < size - 1)
            Serial.print(" | ");
    }
    Serial.println("");
}
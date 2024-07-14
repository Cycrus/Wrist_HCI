/**********************************************************************
 * modules.ino
 * 
 * Testing file for the different modules in the system.
 * 
 * Author: Cyril Marx
 * Created: July 2024
 **********************************************************************/

#include "src/BMI160.hpp"
#include "src/BLE_HID.hpp"

float raw_data[6] = {0};
float data[6] = {0};
BMI160 bmi160;
BLE_HID input_device;

void setup()
{
    Serial.begin(9600);
    while (!Serial);
    input_device.initService("Cyber Finger");
    Wire.begin();
    bmi160.configureBMI160();
}

void loop()
{
    /*bmi160.fetchSensorData();
    bmi160.getRawData(raw_data);
    bmi160.getProcessedData(data);

    Serial.print("1 -1");
    Serial.print(" ");
    Serial.print(data[ACC_X]);
    Serial.print(" ");
    Serial.print(data[ACC_Y]);
    Serial.print(" ");
    Serial.print(data[ACC_Z]);
    Serial.print(" ");
    Serial.print(data[GYR_X]);
    Serial.print(" ");
    Serial.print(data[GYR_Y]);
    Serial.print(" ");
    Serial.println(data[GYR_Z]);

    delay(10);*/

    if(input_device.checkRemoteAvailability())
    {
        while(input_device.checkRemoteConnection())
        {
            while(Serial.available())
            {
                char debug_input = Serial.read();
                if(debug_input == 'a')
                {
                    Serial.println("Pressing A.");
                    input_device.setKeyboardButtonPress('a', MOD_NONE);
                }
                if(debug_input == 'm')
                {
                    Serial.println("Clicking left.");
                    input_device.setMouseButtonPress(MOUSE_LEFT);
                }
                if(debug_input == 'u')
                {
                    Serial.println("Moving Mouse Up.");
                    input_device.setMouseMove(0, -40);
                }
                if(debug_input == 'w')
                {
                    Serial.println("Scrolling Down.");
                    input_device.setMouseScroll(MOUSE_SCROLL_DOWN);
                }
            }

            input_device.sendKeyboardMessage();
            input_device.sendMouseMessage();
            delay(100);
        }
    }
}

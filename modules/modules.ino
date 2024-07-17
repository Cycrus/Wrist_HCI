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
#include "src/ButtonMatrix.hpp"

#define MAX_MOVEMENT_STRENGHT 64
#define MOUSE_MOVEMENT_GAIN 200.0

#define BUTTON_ROW_1 12
#define BUTTON_ROW_2 10
#define BUTTON_COL_1 8
#define BUTTON_COL_2 6

float data[6] = {0};
BMI160 bmi160;
BLE_HID input_device;
ButtonMatrix buttons;

void setup()
{
    Serial.begin(9600);
    while (!Serial);
    input_device.initService("Cyber Device");
    Wire.begin();
    bmi160.configureBMI160();

    buttons.addRowPin(BUTTON_ROW_1);
    buttons.addRowPin(BUTTON_ROW_2);
    buttons.addColPin(BUTTON_COL_1);
    buttons.addColPin(BUTTON_COL_2);
}

void loop()
{
    //bmi160.testRoutine(true);
    //input_device.testRoutine();

    if(input_device.checkRemoteAvailability())
    {
        while(input_device.checkRemoteConnection())
        {
            bmi160.fetchSensorData();
            bmi160.getProcessedData(data);

            int16_t x_movement = data[GYR_Z] * MOUSE_MOVEMENT_GAIN * -1.0;
            int16_t y_movement = data[GYR_X] * MOUSE_MOVEMENT_GAIN;
            float click_movement = data[GYR_Y];

            if(x_movement > MAX_MOVEMENT_STRENGHT)
                x_movement = MAX_MOVEMENT_STRENGHT;
            if(x_movement < -MAX_MOVEMENT_STRENGHT)
                x_movement = -MAX_MOVEMENT_STRENGHT;
            if(y_movement > MAX_MOVEMENT_STRENGHT)
                y_movement = MAX_MOVEMENT_STRENGHT;
            if(y_movement < -MAX_MOVEMENT_STRENGHT)
                y_movement = -MAX_MOVEMENT_STRENGHT;

            /*Serial.print(x_movement);
            Serial.print(" ");
            Serial.println(y_movement);*/

            input_device.setMouseMove(x_movement, y_movement);

            /*if(click_movement > 0.1)
            {
                Serial.println("Left Click");
                input_device.setMouseButtonPress(MOUSE_LEFT);
            }
            else if(click_movement < -0.1)
            {
                Serial.println("Right Click");
                input_device.setMouseButtonPress(MOUSE_RIGHT);
            }*/

            buttons.fetchButtonPresses();

            if(buttons.checkButtonPress(0, 0))
            {
                Serial.println("Button up");
                input_device.setKeyboardButtonPress('w', MOD_LEFT_CTR | MOD_LEFT_ALT);
            }
            if(buttons.checkButtonPress(0, 1))
            {
                Serial.println("Button down");
                input_device.setKeyboardButtonPress('s', MOD_LEFT_CTR | MOD_LEFT_ALT);
            }
            if(buttons.checkButtonPress(1, 0))
            {
                Serial.println("Button left");
                input_device.setKeyboardButtonPress('a', MOD_LEFT_CTR | MOD_LEFT_ALT);
            }
            if(buttons.checkButtonPress(1, 1))
            {
                Serial.println("Button right");
                input_device.setKeyboardButtonPress('d', MOD_LEFT_CTR | MOD_LEFT_ALT);
            }

            input_device.sendKeyboardMessage();
            input_device.sendMouseMessage();
        }
    }
}

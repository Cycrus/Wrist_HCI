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

#define MAX_MOVEMENT_STRENGHT 64

#define BUTTON_MOVE_UP 12
#define BUTTON_MOVE_LEFT 10
#define BUTTON_MOVE_RIGHT 8
#define BUTTON_MOVE_DOWN 6

float data[6] = {0};
BMI160 bmi160;
BLE_HID input_device;

void setup()
{
    Serial.begin(9600);
    while (!Serial);
    input_device.initService("Cyber Device");
    Wire.begin();
    bmi160.configureBMI160();

    pinMode(BUTTON_MOVE_UP, INPUT);
    pinMode(BUTTON_MOVE_LEFT, INPUT);
    pinMode(BUTTON_MOVE_RIGHT, INPUT);
    pinMode(BUTTON_MOVE_DOWN, INPUT);
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

            int16_t x_movement = data[GYR_Z] * 200.0 * -1.0;
            int16_t y_movement = data[GYR_X] * 200.0;
            float click_movement = data[GYR_Y];

            if(x_movement > MAX_MOVEMENT_STRENGHT)
                x_movement = MAX_MOVEMENT_STRENGHT;
            if(x_movement < -MAX_MOVEMENT_STRENGHT)
                x_movement = -MAX_MOVEMENT_STRENGHT;
            if(y_movement > MAX_MOVEMENT_STRENGHT)
                y_movement = MAX_MOVEMENT_STRENGHT;
            if(y_movement < -MAX_MOVEMENT_STRENGHT)
                y_movement = -MAX_MOVEMENT_STRENGHT;

            Serial.print(x_movement);
            Serial.print(" ");
            Serial.println(y_movement);

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

            int button_up = digitalRead(BUTTON_MOVE_UP);
            int button_right = digitalRead(BUTTON_MOVE_RIGHT);
            int button_down = digitalRead(BUTTON_MOVE_DOWN);
            int button_left = digitalRead(BUTTON_MOVE_LEFT);

            if(digitalRead(BUTTON_MOVE_UP) == HIGH)
            {
                input_device.setKeyboardButtonPress('w', MOD_LEFT_CTR | MOD_LEFT_ALT);
            }
            if(digitalRead(BUTTON_MOVE_DOWN) == HIGH)
            {
                input_device.setKeyboardButtonPress('s', MOD_LEFT_CTR | MOD_LEFT_ALT);
            }
            if(digitalRead(BUTTON_MOVE_LEFT) == HIGH)
            {
                input_device.setKeyboardButtonPress('a', MOD_LEFT_CTR | MOD_LEFT_ALT);
            }
            if(digitalRead(BUTTON_MOVE_RIGHT) == HIGH)
            {
                input_device.setKeyboardButtonPress('d', MOD_LEFT_CTR | MOD_LEFT_ALT);
            }

            input_device.sendKeyboardMessage();
            input_device.sendMouseMessage();
        }
    }
}

/**********************************************************************
 * modules.ino
 * 
 * Testing file for the different modules in the system.
 * 
 * Author: Cyril Marx
 * Created: July 2024
 **********************************************************************/

#include "src/BMI160.hpp"

float raw_data[6] = {0};
float data[6] = {0};
BMI160 bmi160;

void setup()
{
    Serial.begin(9600);
    Wire.begin();
    bmi160.configureBMI160();
}

void loop()
{
    bmi160.fetchSensorData();
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

    delay(10);
}

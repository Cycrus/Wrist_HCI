/**********************************************************************
 * BMI160.cpp
 * 
 * Implementation of the BMI160 class.
 * 
 * Author: Cyril Marx
 * Created: July 2024
 **********************************************************************/

#include "BMI160.hpp"

BMI160::BMI160() :
_curr_n{0}
{ }

void BMI160::configureBMI160()
{
  __writeRegister(__BMI160_CMD, 0x11);  // Set accelerometer to normal mode
  delay(10);
  __writeRegister(__BMI160_ACC_RANGE, 0b00000011);  // Set accelerometer range to 2G
  delay(10);
  __writeRegister(__BMI160_ACC_CONF, 0b00101000);  // Set accelerometer output data rate to 100Hz
  delay(10);

  __writeRegister(__BMI160_CMD, 0x15);  // Set gyroscope to normal mode
  delay(10);
  __writeRegister(__BMI160_GYR_RANGE, 0b00000000);  // Set gyroscope range to 2000 degree/s 
  delay(10);
  __writeRegister(__BMI160_GYR_CONF, 0b00101000);  // Set gyroscope output data rate to 100Hz
  delay(10);
}

void BMI160::fetchSensorData()
{
    int8_t prev_n = __getPrevN(_curr_n);

    __readOutputData(_raw_data[_curr_n]);
    __normalizeData(_raw_data[_curr_n], _filtered_data[_curr_n]);
    __filterData(_filtered_data[_curr_n], _filtered_data[prev_n], _filtered_data[_curr_n],
                 (float)ALPHA_HIGH, (float)ALPHA_LOW);
    __sqrRootData(_filtered_data[_curr_n], _filtered_data[_curr_n]);

    __groundData(_filtered_data, _grounded_data[_curr_n]);
    __smoothData(_grounded_data, _final_data[_curr_n]);

    __computeGradient(_final_data[_curr_n], _final_data[prev_n], _grad_data[_curr_n]);

    _curr_n = __getNextN(_curr_n);
}

int16_t BMI160::readMetaData(uint8_t api_reg)
{
  int16_t data = 0;

  switch(api_reg)
  {
    case BMI160_CHIPID:
      data = __read8(__BMI160_CHIP_ID);
      break;

    case BMI160_ERROR:
      data = __read8(__BMI160_ERROR);
      break;

    case BMI160_PMU_STATUS:
      data = __read8(__BMI160_PMU_STATUS);
      break;

    default:
      Serial.println("[BMI160 ERROR] Faulty API register.");
      data = -1;
  }

  return data;
}

void BMI160::getRawData(float output[6])
{
    int8_t fetched_data = __getPrevN(_curr_n);
    for(int i = 0; i < 6; i++)
        output[i] = _raw_data[fetched_data][i];
}

void BMI160::getProcessedData(float output[6])
{
    int8_t fetched_data = __getPrevN(_curr_n);
    for(int i = 0; i < 6; i++)
        output[i] = _final_data[fetched_data][i];
}

void BMI160::getGradientData(float output[6])
{
    int8_t fetched_data = __getPrevN(_curr_n);
    for(int i = 0; i < 6; i++)
        output[i] = _grad_data[fetched_data][i];
}

void BMI160::testRoutine(bool filtered)
{
    float raw_data[6] = {0};
    float data[6] = {0};

    while(true)
    {
        fetchSensorData();
        getRawData(raw_data);
        getProcessedData(data);

        if(filtered)
        {
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
        }
        else
        {
            Serial.print("1 -1");
            Serial.print(" ");
            Serial.print(raw_data[ACC_X]);
            Serial.print(" ");
            Serial.print(raw_data[ACC_Y]);
            Serial.print(" ");
            Serial.print(raw_data[ACC_Z]);
            Serial.print(" ");
            Serial.print(raw_data[GYR_X]);
            Serial.print(" ");
            Serial.print(raw_data[GYR_Y]);
            Serial.print(" ");
            Serial.println(raw_data[GYR_Z]);
        }

        delay(10);
    }
}

int8_t BMI160::__getNextN(int8_t n, uint8_t steps)
{
    int8_t next_n = n + steps;
    if(next_n >= SMOOTH_WINDOW_N)
    {
        next_n = next_n - SMOOTH_WINDOW_N;
    }
    return next_n;
}

int8_t BMI160::__getPrevN(int8_t n, uint8_t steps)
{
    int8_t prev_n = n - steps;
    if(prev_n < 0)
    {
        prev_n = prev_n + SMOOTH_WINDOW_N;
    }
    return prev_n;
}

void BMI160::__writeRegister(uint8_t reg, uint8_t value)
{
    Wire.beginTransmission(BMI160_ADDRESS);
    Wire.write(reg);
    Wire.write(value);
    Wire.endTransmission();
}

int8_t BMI160::__read8(uint8_t reg)
{
    Wire.beginTransmission(BMI160_ADDRESS);
    Wire.write(reg);
    Wire.endTransmission(false);
    Wire.requestFrom(BMI160_ADDRESS, 1);

    int8_t value = 0;
    if(Wire.available())
        value = Wire.read();
    return value;
}

void BMI160::__readOutputData(int16_t buffer[6])
{
    Wire.beginTransmission(BMI160_ADDRESS);
    Wire.write(__BMI160_OUTPUT_REG);
    Wire.endTransmission(false);
    Wire.requestFrom(BMI160_ADDRESS, 20);

    while(Wire.available() != 20);

    uint8_t data[20];
    for(int i = 0; i < 20; i++) {
        data[i] = Wire.read();
    }

    // Gyroscope data
    buffer[GYR_X] = (int16_t)((data[9] << 8) | data[8]);
    buffer[GYR_Y] = (int16_t)((data[11] << 8) | data[10]);
    buffer[GYR_Z] = (int16_t)((data[13] << 8) | data[12]);

    // Accelerometer data
    buffer[ACC_X] = (int16_t)((data[15] << 8) | data[14]);
    buffer[ACC_Y] = (int16_t)((data[17] << 8) | data[16]);
    buffer[ACC_Z] = (int16_t)((data[19] << 8) | data[18]);
}

void BMI160::__normalizeData(int16_t input_data[6], float output_data[6])
{
    output_data[GYR_X] = input_data[GYR_X] * 3.14 / 180.0 / 150.0;
    output_data[GYR_Y] = input_data[GYR_Y] * 3.14 / 180.0 / 150.0;
    output_data[GYR_Z] = input_data[GYR_Z] * 3.14 / 180.0 / 150.0;

    output_data[ACC_X] = input_data[ACC_X] / 16384.0;
    output_data[ACC_Y] = input_data[ACC_Y] / 16384.0;
    output_data[ACC_Z] = input_data[ACC_Z] / 16384.0;

    // To fix the constant gravity offset from the z axis.
    output_data[ACC_Z] -= 1.0;
}

void BMI160::__filterData(float input_data[6], float prev_input_data[6], float output_data[6],
                          float alpha_high, float alpha_low)
{
  for(int i = 0; i < 6; i++)
  {
    // High pass filter
    output_data[i] = alpha_high * (prev_input_data[i] + input_data[i] - prev_input_data[i]);

    // Low pass filter
    output_data[i] = alpha_low * input_data[i] + (1 - alpha_low) * prev_input_data[i];
  }
}

void BMI160::__sqrRootData(float input_data[6], float output_data[6])
{
    for(int i = 3; i < 6; i++)
    {
        bool is_neg = input_data[i] < 0;
        int factor = 1;
        if(is_neg) factor = -1;

        output_data[i] = sqrt(input_data[i] * factor) * factor;
    }
}

void BMI160::__groundData(float input_data[][6], float output_data[])
{
    float sum_value[6] = {0};
    for(int i = 0; i < SMOOTH_WINDOW_N; i++)
    {
        for(int data_id = 3; data_id < 6; data_id++)
        {
            sum_value[data_id] += input_data[i][data_id];
        }

    }

    for(int data_id = 0; data_id < 3; data_id++)
    {
        output_data[data_id] = input_data[_curr_n][data_id];
    }    
    for(int data_id = 3; data_id < 6; data_id++)
    {
        float average_value = sum_value[data_id] / SMOOTH_WINDOW_N;
        output_data[data_id] = input_data[_curr_n][data_id] - average_value;
    }
}

void BMI160::__smoothData(float input_data[][6], float output_data[])
{
    float sum_value[6] = {0};
    for(int i = 0; i < SMOOTH_WINDOW_N; i++)
    {
        for(int data_id = 0; data_id < 6; data_id++)
        {
            sum_value[data_id] += input_data[i][data_id];
        }

    }
    
    for(int data_id = 0; data_id < 6; data_id++)
    {
        float average_value = sum_value[data_id] / SMOOTH_WINDOW_N;
        output_data[data_id] = average_value;
    }
}

void BMI160::__computeGradient(float input_data[6], float prev_input_data[6], float output_data[6])
{
    for(int i = 0; i < 6; i++)
    {
        output_data[i] = prev_input_data[i] - input_data[i];
    }
}
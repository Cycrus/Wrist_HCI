#include <Arduino.h>
#include <Wire.h>

#define BMI160_ADDRESS 0x69

// API Level registers
#define BMI160_ERROR 0x00

#define BMI160_GYR_X 0x01
#define BMI160_GYR_Y 0x02
#define BMI160_GYR_Z 0x03

#define BMI160_ACC_X 0x04
#define BMI160_ACC_Y 0x05
#define BMI160_ACC_Z 0x06

#define BMI160_CHIPID 0x08
#define BMI160_PMU_STATUS 0x09

// Chip data low level registers
#define __BMI160_CHIP_ID 0x00
#define __BMI160_ERROR 0x02
#define __BMI160_PMU_STATUS 0x03

#define __BMI160_OUTPUT_REG 0x04

#define __BMI160_GYR_X_LSB 0x0C
#define __BMI160_GYR_X_MSB 0x0D
#define __BMI160_GYR_Y_LSB 0x0E
#define __BMI160_GYR_Y_MSB 0x0F
#define __BMI160_GYR_Z_LSB 0x10
#define __BMI160_GYR_Z_MSB 0x11

#define __BMI160_ACC_X_LSB 0x12
#define __BMI160_ACC_X_MSB 0x13
#define __BMI160_ACC_Y_LSB 0x14
#define __BMI160_ACC_Y_MSB 0x15
#define __BMI160_ACC_Z_LSB 0x16
#define __BMI160_ACC_Z_MSB 0x17

#define __BMI160_ACC_CONF 0x40
#define __BMI160_ACC_RANGE 0x41
#define __BMI160_GYR_CONF 0x42
#define __BMI160_GYR_RANGE 0x43
#define __BMI160_CMD 0x7E

#define ALPHA_HIGH 0.5
#define ALPHA_LOW 0.9

#define SMOOTH_WINDOW_N 20;

u_int8_t window_n = 0;
int16_t raw_data[6][SMOOTH_WINDOW_N] = {0};
float filtered_data[6][SMOOTH_WINDOW_N] = {0};
float grad_data[6][SMOOTH_WINDOW_N] = {0};



void setup()
{
  Serial.begin(9600);
  Wire.begin();
  configureBMI160();
}

void loop()
{
  readSensorData(raw_data);
  normalizeData(raw_data, filtered_data);
  float ax = sensor_data[0];
  float ay = sensor_data[1];
  float az = sensor_data[2];
  float gx = sensor_data[3];
  float gy = sensor_data[4];
  float gz = sensor_data[5];

  // Correct natural, constant bias due to gravity
  az -= 0.5;

  // High pass filter
  float filteredAccX = ALPHA_HIGH * (prevAccX + ax - prevAccX);
  float filteredAccY = ALPHA_HIGH * (prevAccY + ay - prevAccY);
  float filteredAccZ = ALPHA_HIGH * (prevAccZ + az - prevAccZ);

  // Low pass filter
  filteredAccX = ALPHA_LOW * filteredAccX + (1 - ALPHA_LOW) * prevAccX;
  filteredAccY = ALPHA_LOW * filteredAccY + (1 - ALPHA_LOW) * prevAccY;
  filteredAccZ = ALPHA_LOW * filteredAccZ + (1 - ALPHA_LOW) * prevAccZ;

  // Compute differentials
  ax_diff = prevAccX - filteredAccX;
  ay_diff = prevAccY - filteredAccY;
  az_diff = prevAccZ - filteredAccZ;

  // Update previous values
  prevAccX = filteredAccX;
  prevAccY = filteredAccY;
  prevAccZ = filteredAccZ;

  Serial.print(filteredAccX);
  Serial.print(" ");
  Serial.print(filteredAccY);
  Serial.print(" ");
  Serial.print(filteredAccZ);
  Serial.print(" ");
  Serial.print(ax_diff);
  Serial.print(" ");
  Serial.print(ay_diff);
  Serial.print(" ");
  Serial.println(az_diff);
  /*Serial.print(" ");
  Serial.print(gx);
  Serial.print(" ");
  Serial.print(gy);
  Serial.print(" ");
  Serial.println(gz);*/

  delay(66);
}

void normalizeData(int16_t[] raw_data, float[] normalized_data)
{
  sensor_data[0] = acc_x / 16384.0;
  sensor_data[1] = acc_y / 16384.0;
  sensor_data[2] = acc_z / 16384.0;
  sensor_data[3] = gyr_x * 3.14 / 180.0;
  sensor_data[4] = gyr_y * 3.14 / 180.0;
  sensor_data[5] = gyr_z * 3.14 / 180.0;
}

void configureBMI160()
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

void readSensorData(int16_t& acc_x, int16_t& acc_y, int16_t& acc_z,
                    int16_t& gyr_x, int16_t& gyr_y, int16_t& gyr_z)
{
  __readOutputData(acc_x, acc_y, acc_z, gyr_x, gyr_y, gyr_z);
}

int16_t readMetaData(uint8_t api_reg)
{
  int16_t data = 0;
  int16_t ldata = 0;
  int16_t mdata = 0;

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

    case BMI160_GYR_X:
      ldata = __read8(__BMI160_GYR_X_LSB);
      mdata = __read8(__BMI160_GYR_X_MSB);
      data = ldata;
      data |= mdata << 8;
    break;

    case BMI160_GYR_Y:
      ldata = __read8(__BMI160_GYR_Y_LSB);
      mdata = __read8(__BMI160_GYR_Y_MSB);
      data = ldata;
      data |= mdata << 8;
    break;

    case BMI160_GYR_Z:
      ldata = __read8(__BMI160_GYR_Z_LSB);
      mdata = __read8(__BMI160_GYR_Z_MSB);
      data = ldata;
      data |= mdata << 8;
    break;

    case BMI160_ACC_X:
      ldata = __read8(__BMI160_ACC_X_LSB);
      mdata = __read8(__BMI160_ACC_X_MSB);
      data = ldata;
      data |= mdata << 8;
    break;

    case BMI160_ACC_Y:
      ldata = __read8(__BMI160_ACC_Y_LSB);
      mdata = __read8(__BMI160_ACC_Y_MSB);
      data = ldata;
      data |= mdata << 8;
    break;

    case BMI160_ACC_Z:
      ldata = __read8(__BMI160_ACC_Z_LSB);
      mdata = __read8(__BMI160_ACC_Z_MSB);
      data = ldata;
      data |= mdata << 8;
    break;

    default:
      Serial.println("[BMI160 ERROR] Faulty API register.");
      data = -1;
  }

  return data;
}

void __writeRegister(uint8_t reg, uint8_t value)
{
  Wire.beginTransmission(BMI160_ADDRESS);
  Wire.write(reg);
  Wire.write(value);
  Wire.endTransmission();
}

int8_t __read8(uint8_t reg)
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

void __readOutputData(int16_t& acc_x, int16_t& acc_y, int16_t& acc_z,
                      int16_t& gyr_x, int16_t& gyr_y, int16_t& gyr_z)
{
  Wire.beginTransmission(BMI160_ADDRESS);
  Wire.write(0x04);
  Wire.endTransmission(false);
  Wire.requestFrom(BMI160_ADDRESS, 20);
  
  while(Wire.available() != 20);

  uint8_t data[20];
  for(int i = 0; i < 20; i++) {
    data[i] = Wire.read();
  }

  // Gyroscope data
  gyr_x = (int16_t)((data[9] << 8) | data[8]);
  gyr_y = (int16_t)((data[11] << 8) | data[10]);
  gyr_z = (int16_t)((data[13] << 8) | data[12]);

  // Accelerometer data
  acc_x = (int16_t)((data[15] << 8) | data[14]);
  acc_y = (int16_t)((data[17] << 8) | data[16]);
  acc_z = (int16_t)((data[19] << 8) | data[18]);
}

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
#define __BMI160_CMD 0x7E


void setup()
{
  Serial.begin(9600);
  Wire.begin();
  configureBMI160();
}

void loop()
{
  uint16_t pmu_status = readData(BMI160_PMU_STATUS);
  Serial.print("PMU Status = ");
  Serial.println(pmu_status);

  uint16_t error_code = readData(BMI160_ERROR);
  Serial.print("ERROR Code = ");
  Serial.println(error_code);

  delay(1000);
}

void configureBMI160()
{
  // Set accelerometer configuration (example configuration)
  __writeRegister(__BMI160_CMD, 0x11);  // Set accelerometer to normal mode
  delay(10);
  __writeRegister(__BMI160_CMD, 0x15);  // Set gyroscope to normal mode
  delay(10);
  __writeRegister(__BMI160_ACC_RANGE, 0b00000101);  // Set accelerometer range to 4G
  delay(10);
  __writeRegister(__BMI160_ACC_CONF, 0b00101000);  // Set output data rate to 100Hz
  delay(10);
}

int16_t readData(uint8_t api_reg)
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

/**********************************************************************
 * BMI160.hpp
 * 
 * A specialized class to retrieve sensor data from the BMI160
 * accelerometer/gyroscope module.
 * Filters and processes data for better use.
 * 
 * Author: Cyril Marx
 * Created: July 2024
 **********************************************************************/

#ifndef BMI160_HPP
#define BMI160_HPP


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

#define __BMI160_ACC_CONF 0x40
#define __BMI160_ACC_RANGE 0x41
#define __BMI160_GYR_CONF 0x42
#define __BMI160_GYR_RANGE 0x43
#define __BMI160_CMD 0x7E

// Data processing parameters
#define SMOOTH_WINDOW_N 6
#define ALPHA_HIGH 0.7
#define ALPHA_LOW 0.7

#define GYR_X 0
#define GYR_Y 1
#define GYR_Z 2
#define ACC_X 3
#define ACC_Y 4
#define ACC_Z 5

class BMI160
{
    public:
        BMI160();

        void configureBMI160();
        void fetchSensorData();
        int16_t readMetaData(uint8_t api_reg);
        void getRawData(float output[]);
        void getProcessedData(float output[]);
        void getGradientData(float output[]);

    private:
        int8_t _curr_n;
        int16_t _raw_data[SMOOTH_WINDOW_N][6];
        float _filtered_data[SMOOTH_WINDOW_N][6];
        float _grounded_data[SMOOTH_WINDOW_N][6];
        float _final_data[SMOOTH_WINDOW_N][6];
        float _grad_data[SMOOTH_WINDOW_N][6];

        int8_t __getNextN(int8_t n, uint8_t steps = 1);
        int8_t __getPrevN(int8_t n, uint8_t steps = 1);
        void __writeRegister(uint8_t reg, uint8_t value);
        int8_t __read8(uint8_t reg);
        void __readOutputData(int16_t buffer[]);
        void __normalizeData(int16_t input_data[], float output_data[]);
        void __filterData(float input_data[], float prev_input_data[], float output_data[],
                          float alpha_high, float alpha_low);
        void __sqrRootData(float input_data[], float output_data[]);
        void __groundData(float data[][6], float output_data[]);
        void __smoothData(float data[][6], float output_data[]);
        void __computeGradient(float input_data[], float prev_input_data[], float output_data[]);
};

#endif // BMI160_HPP
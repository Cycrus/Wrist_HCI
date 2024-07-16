/**********************************************************************
 * BMI160.hpp
 * 
 * A specialized class to retrieve sensor data from the BMI160
 * accelerometer/gyroscope module.
 * Filters and processes data for better use.
 * Uses a windowed approach which grounds accelerator data to account
 * for gravitational pull and smoothes accelerator and gyroscope data.
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

// IDs of the data in the buffers
#define GYR_X 0
#define GYR_Y 1
#define GYR_Z 2
#define ACC_X 3
#define ACC_Y 4
#define ACC_Z 5

class BMI160
{
    public:
    //-----------------------------------------------------------------------------------------------------------------
    ///
    /// Constructor.
    //
    BMI160();

    //-----------------------------------------------------------------------------------------------------------------
    ///
    /// Configures the module by setting registers to normal mode and setting to ranges to default ranges.
    //
    void configureBMI160();

    //-----------------------------------------------------------------------------------------------------------------
    ///
    /// Fetches the sensor data, processes it and stores it in the buffers.
    //
    void fetchSensorData();

    //-----------------------------------------------------------------------------------------------------------------
    ///
    /// Requests some meta data from the sensor and sends it back.
    ///
    /// @param api_reg  The register to read from (see low level register macros above).
    ///
    /// @return The answer from the sensor given a correct register.
    //
    int16_t readMetaData(uint8_t api_reg);

    //-----------------------------------------------------------------------------------------------------------------
    ///
    /// Returns a copy of the raw, unprocessed data from the sensor.
    ///
    /// @param output  Pointer to an array of size 6 where the data should be stored in.
    //
    void getRawData(float output[6]);

    //-----------------------------------------------------------------------------------------------------------------
    ///
    /// Returns a copy of the processed, filtered data from the sensor.
    ///
    /// @param output  Pointer to an array of size 6 where the data should be stored in.
    //
    void getProcessedData(float output[6]);

    //-----------------------------------------------------------------------------------------------------------------
    ///
    /// Returns a copy of the gradient (change values) of the filtered data.
    ///
    /// @param output  Pointer to an array of size 6 where the data should be stored in.
    //
    void getGradientData(float output[6]);

    //-----------------------------------------------------------------------------------------------------------------
    ///
    /// A test routine to quickly test the functionality of the module. Captures the main loop completely.
    /// Reads the sensor data and prints the data to the Serial output of the Arduino.
    /// Order of data:
    ///   1. constant dummy data to keep range in viewer
    ///   2. constant dummy data to keep range in viewer
    ///   3. accelerator x axis
    ///   4. accelerator y axis
    ///   5. accelerator z axis
    ///   6. gyroscope   x axis
    ///   7. gyroscope   y axis
    ///   8. gyroscope   z axis
    ///
    /// @param filtered     If true prints filtered data, if false prints raw data.
    //
    void testRoutine(bool filtered);

    private:
    int8_t _curr_n;
    int16_t _raw_data[SMOOTH_WINDOW_N][6];
    float _filtered_data[SMOOTH_WINDOW_N][6];
    float _grounded_data[SMOOTH_WINDOW_N][6];
    float _final_data[SMOOTH_WINDOW_N][6];
    float _grad_data[SMOOTH_WINDOW_N][6];

    //-----------------------------------------------------------------------------------------------------------------
    ///
    /// Computes a next valid id in the buffers. Wraps around if it exceeds smoothing window.
    ///
    /// @param n        The id to start from.
    /// @param steps    The steps to add to the given n.
    ///
    /// @return The next valid id in the buffers.
    //
    int8_t __getNextN(int8_t n, uint8_t steps = 1);

    //-----------------------------------------------------------------------------------------------------------------
    ///
    /// Computes a previous valid id in the buffers. Wraps around if it gets lower than 0.
    ///
    /// @param n        The id to start from.
    /// @param steps    The steps to subtract to the given n.
    ///
    /// @return The previous valid id in the buffers.
    //
    int8_t __getPrevN(int8_t n, uint8_t steps = 1);

    //-----------------------------------------------------------------------------------------------------------------
    ///
    /// Writes a value to one of the modules registers.
    ///
    /// @param reg      The register to write in.
    /// @param value    The value to write to the register.
    //
    void __writeRegister(uint8_t reg, uint8_t value);

    //-----------------------------------------------------------------------------------------------------------------
    ///
    /// Reads 1 byte (8 bits) from a certain register.
    ///
    /// @param reg      The register to read from.
    ///
    /// @return The 8 bits answer of the register.
    //
    int8_t __read8(uint8_t reg);

    //-----------------------------------------------------------------------------------------------------------------
    ///
    /// Requests the whole output data and stores it in the given buffer. Needs a buffer of size 6.
    ///
    /// @param buffer   The buffer to write to.
    //
    void __readOutputData(int16_t buffer[6]);

    //-----------------------------------------------------------------------------------------------------------------
    ///
    /// Normalizes the data and brings it to a range of approximatly -1 and 1. Also casts the integer values from the
    /// module to floats.
    ///
    /// @param input_data       The data point to normalize (6 axes).
    /// @param output_data      The array to store the normalized data in (6 axes).
    //
    void __normalizeData(int16_t input_data[6], float output_data[6]);

    //-----------------------------------------------------------------------------------------------------------------
    ///
    /// Uses very rough low and high pass filters to process the data.
    ///
    /// @param input_data           The data point to filter (6 axes).
    /// @param prev_input_data      The previous data point (6 axes).
    /// @param output_data          The array to store the filtered data in (6 axes).
    /// @param alpha_high           The alpha value for the high pass filter.
    /// @param alpha_low            The alpha value for the low pass filter.
    //
    void __filterData(float input_data[6], float prev_input_data[6], float output_data[6],
                        float alpha_high, float alpha_low);

    //-----------------------------------------------------------------------------------------------------------------
    ///
    /// Computes the square root of the accelerator data in a buffer to contrast the values.
    ///
    /// @param input_data           The data point to square (6 axes).
    /// @param output_data          The array to store the squared data in (6 axes).
    //
    void __sqrRootData(float input_data[6], float output_data[6]);

    //-----------------------------------------------------------------------------------------------------------------
    ///
    /// A synthetic force to ground data back to 0 after detecting continuous changes. Acts as automatic calibration
    /// at the cost of not being able to detect constant data changes anymore. Is a method of accounting for gravitational
    /// pull at the module. Is currently only applied to the accelerator data.
    ///
    /// Works by computing the average of the whole data window and subtracting that from the latest data point.
    ///
    /// @param data                 The full data buffer with all data points to ground.
    /// @param output_data          The output buffer for the full grounded data buffer.
    //
    void __groundData(float data[][6], float output_data[]);

    //-----------------------------------------------------------------------------------------------------------------
    ///
    /// Smoothes the data in the given buffer.
    ///
    /// @param data                 The full data buffer with all data points to smooth.
    /// @param output_data          The output buffer for the full smoothed data buffer.
    //
    void __smoothData(float data[][6], float output_data[]);

    //-----------------------------------------------------------------------------------------------------------------
    ///
    /// Computes the discrete differentials gradient of the given data at the latest timepoint.
    ///
    /// @param input_data           The latest data point to use for gradient computation (6 axes).
    /// @param prev_input_data      The previous data point to use for gradient computation (6 axes).
    /// @param output_data          The array to store the gradient data in (6 axes).
    //
    void __computeGradient(float input_data[6], float prev_input_data[6], float output_data[6]);
};

#endif // BMI160_HPP
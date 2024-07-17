/**********************************************************************
* ButtonMatrix.hpp
* 
* A class to allow a button matrix by defining some digital output pins
* which interact with some digital input pins. This allows for more
* buttons than digital GPIO pins on the Arduino.
* 
* Author: Cyril Marx
* Created: July 2024
**********************************************************************/

#ifndef BUTTONMATRIX_HPP
#define BUTTONMATRIX_HPP

#include "Arduino.h"

#define ROWS 2
#define COLS 2

class ButtonMatrix
{
    public:
    ButtonMatrix();

    uint8_t addRowPin(uint8_t pin);
    uint8_t addColPin(uint8_t pin);

    void fetchButtonPresses();
    bool checkButtonPress(uint8_t row, uint8_t col);

    private:
    uint8_t _set_rows;
    uint8_t _set_cols;
    uint8_t _row_pins[ROWS];
    uint8_t _col_pins[COLS];
    bool _button_presses[ROWS][COLS];
};

#endif // BUTTONMATRIX_HPP
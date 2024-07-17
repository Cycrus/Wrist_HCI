/**********************************************************************
* ButtonMatrix.cpp
* 
* Implementation of the ButtonMatrix class.
* 
* Author: Cyril Marx
* Created: July 2024
**********************************************************************/

#include "ButtonMatrix.hpp"

//-----------------------------------------------------------------------------------------------------------------
ButtonMatrix::ButtonMatrix() :
_set_rows(0),
_set_cols(0)
{
    for(int i = 0; i < ROWS; i++)
        _row_pins[i] = 0;
    for(int i = 0; i < COLS; i++)
        _col_pins[i] = 0;
    for(int i = 0; i < ROWS; i++)
        for(int j = 0; j < COLS; j++)
            _button_presses[i][j] = false;
}

//-----------------------------------------------------------------------------------------------------------------
uint8_t ButtonMatrix::addRowPin(uint8_t row)
{
    if(_set_rows >= ROWS)
        return 1;

    _row_pins[_set_rows] = row;
    pinMode(row, OUTPUT);
    digitalWrite(row, HIGH);
    _set_rows++;

    return 0;
}

//-----------------------------------------------------------------------------------------------------------------
uint8_t ButtonMatrix::addColPin(uint8_t col)
{
    if(_set_cols >= COLS)
        return 1;

    _col_pins[_set_cols] = col;
    pinMode(col, INPUT_PULLUP);
    _set_cols++;

    return 0;
}

//-----------------------------------------------------------------------------------------------------------------
void ButtonMatrix::fetchButtonPresses()
{
    for(int row = 0; row < ROWS; row++)
    {
        digitalWrite(_row_pins[row], LOW);
        for(int col = 0; col < COLS; col++)
        {
            if(digitalRead(_col_pins[col]) == LOW)
                _button_presses[row][col] = true;
            else
                _button_presses[row][col] = false;
        }
        digitalWrite(_row_pins[row], HIGH);
    }
}

//-----------------------------------------------------------------------------------------------------------------
bool ButtonMatrix::checkButtonPress(uint8_t row, uint8_t col)
{
    if(row >= ROWS || col >= COLS)
        return false;

    return _button_presses[row][col];
}
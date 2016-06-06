#include "RowScanner_BitManipulation.h"
/*
Strobes the row and reads the columns.
Strobe is on for shortest possible time to preserve IR LED on DodoHand's optic switch.
*/
uint8_t RowScanner_BitManipulation::scan(uint16_t& rowEnd, const bool activeHigh)
{
    //strobe row on
    if (activeHigh)
    {
        refRowPort.setActivePinHigh(rowPin);
    }
    else //activeLow
    {
        refRowPort.setActivePinLow(rowPin);
    }

    //read all the column ports
    for (uint8_t i=0; i < colPortCount; i++)
    {
        ptrsColPorts[i]->read();
    }

    //strobe row off
    if (activeHigh)
    {
        refRowPort.setActivePinLow(rowPin);
    }
    else //activeLow
    {
        refRowPort.setActivePinHigh(rowPin);
    }
   
    return getRowState(rowEnd, activeHigh);
}

/*
Copies column pins to rowState.  Unused column pins are not copied.
Sets rowEnd and returns rowState.
rowEnd is bitwise, where 1 bit corrsiponds to place immediatly after last key of row.
rowEnd and rowMask are larger type than portMask so that they can not overflow.
*/
uint8_t RowScanner_BitManipulation::getRowState(uint16_t& rowEnd, const bool activeHigh)
{
    uint16_t rowMask = 1;           //bitwise, one col per bit, active col bit is 1
    uint8_t rowState = 0;           //bitwise, one key per bit, 1 means key is pressed

    for (uint8_t i=0; i < colPortCount; i++)    //for each col port
    {
        //bitwise colPins, 1 means pin is connected to column
        uint8_t colPins = ptrsColPorts[i]->getColPins();

        //bitwise colPortState, pin values where set in ColPort::read(), get them now
        uint8_t colPortState = ptrsColPorts[i]->getPortState();

        if (activeHigh)
        {
            colPortState = ~colPortState;
        }

        for ( uint8_t portMask = 1; portMask > 0; portMask <<= 1 ) //shift portMask until overflow
        {                                       //for each pin of col port
            if (portMask & colPins)             //if pin is connected to column
            {
                if (portMask & ~colPortState)   //if pin detected a key press
                {
                    rowState |= rowMask;        //set rowState bit for that key
                }

                rowMask <<= 1;                  //shift rowMask to next key
            }
        }
    }

    rowEnd = rowMask;

    return rowState;
}

// Copyright 2008 Peter Stegemann

#pragma once

#include "System_Ports.h"

#include "Types.h"

// 00 10 11 01 (0 2 3 1) clockwise
// 00 01 11 10 (0 1 3 2) anticlockwise
static const int8_t RotaryTable[] =
{
	// 1111 1110 1101 1100
	//   00   01   10   11
	      0,  +1,  -1,   0,
	// 1011 1010 1001 1000
	//   01   11   11   10
	     -1,   0,   0,  +1,
	// 0111 0110 0101 0100
	//   10   11   00   01
	     +1,   0,   0,  -1,
	// 0011 0010 0001 0000
	//   11   10   01   00
	      0,  -1,  +1,   0
};

class Input_Rotary
{
  private:
    int8_t rotarySubValue;
	  uint8_t lastRotarySelect;
	  uint8_t triggerValue;

  public:
  	// Pass in the idle values of the rotary here.
	  void Initialize( bool InputA, bool InputB)
	  {
    	rotarySubValue = 0;
	    lastRotarySelect = 0;

    	// We're trying to get the opposite of the idle position by inverting the values we get.
	    triggerValue = 0;

    	if( InputA == false)	triggerValue |= 0x01;
	    if( InputB == false)	triggerValue |= 0x02;
    }

    int8_t CalculateDifference( bool InputA, bool InputB)
    {
    	int8_t Result = 0;

    	// Rotary select
	    uint8_t NewRotarySelect = 0;

	    if( InputA == true)	NewRotarySelect |= 0x01;
    	if( InputB == true)	NewRotarySelect |= 0x02;

    	if( NewRotarySelect != lastRotarySelect)
	    {
    		int8_t Step = RotaryTable[( lastRotarySelect << 2) | NewRotarySelect];
    		rotarySubValue += Step;

    		if( NewRotarySelect == triggerValue)
	    	{
	    		if( rotarySubValue > 0)
		    	{
				    Result++;
			    }
    			else if( rotarySubValue < 0)
	    		{
    				Result--;
	    		}

    			rotarySubValue = 0;
	    	}

    		lastRotarySelect = NewRotarySelect;
	    }

    	return( Result);
    }
};

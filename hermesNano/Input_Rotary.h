// Copyright 2008 Peter Stegemann

#pragma once

#include "Types.h"

// 00 10 11 01 (0 2 3 1) clockwise
// 00 01 11 10 (0 1 3 2) anticlockwise

// 1111 1110 1101 1100
//    0,  +1,  -1,   0,
// 1011 1010 1001 1000
//    0,   0,   0,   0,
// 0111 0110 0101 0100
//    0,   0,   0,   0,
// 0011 0010 0001 0000
//    0,  -1,  +1,   0

class Input_Rotary
{
  private:
    int8_t subSteps;
	  uint8_t lastBits;

  public:
	  Input_Rotary( void)
    	: subSteps( 0)
	    , lastBits( 0)
	  {
    }

    int8_t CalculateDifference( bool InputA, bool InputB)
    {
    	// Rotary select
	    uint8_t NewBits = InputB << 1 | InputA;

      if( NewBits == lastBits)
      {
        return( 0);
      }

      uint8_t State = ( lastBits << 2) | NewBits;

      lastBits = NewBits;

      // Step on every second step, 90 degree before and after the resting position.
      if(( State == 0b0010) || ( State == 0b1101))
      {
        subSteps--;
      }
      else if(( State == 0b0001) || ( State == 0b1110))
      {
        subSteps++;
      }
      // Reset a glitched rotary encoder decoder in the resting position.
      else if( NewBits == 0b11)
      {
        subSteps = 0;
      }

      // Two sub steps make one step.
      if( subSteps >= 2)
      {
        subSteps = 0;

        return( 1);
      }
      else if( subSteps <= -2)
      {
        subSteps = 0;
 
        return( -1);
      }
      else
      {
        return( 0);
      }
    }

    int8_t GetSubSteps( void)
    {
      return( subSteps);
    }
};

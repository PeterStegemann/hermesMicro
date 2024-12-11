// Copyright 2010 Peter Stegemann

#pragma once

#include "Input_Service.h"
#include "Setup_Service.h"
#include "Setup_Trim.h"
#include "Signal_Defines_Trim.h"
#include "Status_Engine.h"

class Signal_Output
{
	public:
		Setup_Output Setup;

	public:
		int16_t Calculate( int16_t Value)
    {
      // Move value into positive realm.
      uint16_t UnsignedValue = ( int32_t) Value - ( int32_t) SIGNAL_MINIMUM_VALUE;

      // Calculate width of one map column.
      uint16_t Width = (( uint16_t) SIGNAL_VALUE_RANGE) / 2;

      // Calculate in which column this value is.
      int32_t Lower, Higher;

      // Lower half.
      if( Value < 0)
      {
        Lower = Setup.Bottom;
        Higher = Setup.Center;
      }
      // Upper half.
      else if( Value < SIGNAL_MAXIMUM_VALUE)
      {
        Lower = Setup.Center;
        Higher = Setup.Top;
      }
      // Top point.
      else
      {
        int32_t Result = Setup.Top;
        Result *= SIGNAL_VALUE_RANGE;
        Result /= SIGNAL_TRIM_VALUE_RANGE;

        if( Setup.Reverse)
        {
          Result = -Result;
        }

        return( Result);
      }

      // Blow up to signal level.
      Lower *= SIGNAL_VALUE_RANGE;
      Lower /= SIGNAL_TRIM_VALUE_RANGE;
      Higher *= SIGNAL_VALUE_RANGE;
      Higher /= SIGNAL_TRIM_VALUE_RANGE;

      // Now calculate where in the column it is.
      int32_t Where = UnsignedValue % Width;

      int32_t Rise = Higher - Lower;
      int32_t Full = Where * Rise;
      Full /= Width;

      int16_t Result = Lower + Full;

      if( Setup.Reverse)
      {
        Result = -Result;
      }

      return( Result);
    }
};

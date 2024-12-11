// Copyright 2008 Peter Stegemann

#pragma once

#include "Setup_Calibration.h"
#include "Signal_Defines.h"

class Signal_Calibration
{
	public:
		Setup_Calibration Setup;

	public:
		Signal_Calibration( void)
    {
      Setup.High = SIGNAL_CALIBRATION_MAXIMUM_VALUE;
      Setup.Center = SIGNAL_CALIBRATION_NEUTRAL_VALUE;
      Setup.Low = SIGNAL_CALIBRATION_MINIMUM_VALUE;
    }

		int16_t Calibrate( int16_t Value)
    {
      if( Value < Setup.Low)
      {
        Value = Setup.Low;
      }
      else if( Value > Setup.High)
      {
        Value = Setup.High;
      }

      if(( Setup.Center == Setup.Low) || ( Setup.Center == Setup.High))
      {
        // Two point calibration.
        int16_t ValueRange = Setup.High - Setup.Low;

        if( ValueRange == 0)
        {
          // No range, center value.
          return( SIGNAL_NEUTRAL_VALUE);
        }
        else
        {
          Value -= Setup.Low;

          int32_t CalibratedValue = Value;
          CalibratedValue *= SIGNAL_VALUE_RANGE;
          CalibratedValue /= ValueRange;
          CalibratedValue += SIGNAL_MINIMUM_VALUE;

          return( CalibratedValue);
        }
      }
      else
      {
        int16_t ValueRange;
        int16_t ValueCenter = 0;

        if( Setup.Low < Setup.High)
        {
          if( Value <= Setup.Center)
          {
            if( Value <= Setup.Low)
            {
              Value = Setup.Low;
            }

            Value -= Setup.Low;

            ValueRange = Setup.Center - Setup.Low;
            ValueCenter = SIGNAL_MINIMUM_VALUE;
          }
          else
          {
            if( Value >= Setup.High)
            {
              Value = Setup.High;
            }

            Value -= Setup.Center;

            ValueRange = Setup.High - Setup.Center;
          }
        }
        else
        {
          if( Value <= Setup.Center)
          {
            if( Value <= Setup.High)
            {
              Value = Setup.High;
            }

            Value -= Setup.High;

            ValueRange = -( Setup.Center - Setup.High);
            ValueCenter = SIGNAL_MAXIMUM_VALUE;
          }
          else
          {
            if( Value >= Setup.Low)
            {
              Value = Setup.Low;
            }

            Value -= Setup.Center;

            ValueRange = -( Setup.Low - Setup.Center);
          }
        }

        int32_t CalibratedValue = Value;
        CalibratedValue *= SIGNAL_VALUE_RANGE / 2;
        CalibratedValue /= ValueRange;
        CalibratedValue += ValueCenter;

        return( CalibratedValue);
      }
    }
};

// Copyright 2007 Peter Stegemann

#pragma once

#include "Setup.h"
#include "Types.h"

#include <avr/eeprom.h>

class Setup_Calibration;
class Setup_Trim;

extern Setup Eeprom EEMEM;

class Setup_Service
{
	public:
    void Reset( void)
    {
      SetInvertedPPM( false);

      for( uint8_t Index = 0; Index < INPUT_ANALOG_PORTS; Index++)
      {
        ResetCalibration( Index);
      }

      for( uint8_t Index = 0; Index < SIGNAL_TRIMS; Index++)
      {
        ResetTrim( Index);
      }

      for( uint8_t Index = 0; Index < OUTPUT_CHANNELS; Index++)
      {
        ResetOutput( Index);
      }
    }

    void ResetCalibration( uint8_t Index)
    {
      Setup_Calibration DefaultCalibration =
      {
        SIGNAL_CALIBRATION_MAXIMUM_VALUE,
        SIGNAL_CALIBRATION_NEUTRAL_VALUE,
        SIGNAL_CALIBRATION_MINIMUM_VALUE
      };

      SetCalibration( Index, &DefaultCalibration);
    }

    void SetCalibration( uint8_t Index, Setup_Calibration* Calibration)
    {
      if( Index >= INPUT_ANALOG_PORTS)
      {
        // That index is not available.
        return;
      }

      eeprom_write_block( Calibration, &( Eeprom.Calibration[ Index]), sizeof( Setup_Calibration));
    }

    void GetCalibration( uint8_t Index, Setup_Calibration* Calibration)
    {
      if( Index >= INPUT_ANALOG_PORTS)
      {
        // That index is not available.
        return;
      }

      eeprom_read_block( Calibration, &( Eeprom.Calibration[ Index]), sizeof( Setup_Calibration));
    }

    void ResetTrim( uint8_t Index)
    {
      Setup_Trim DefaultTrim =
      {
        SIGNAL_TRIM_NEUTRAL_VALUE,
      };

      SetTrim( Index, &DefaultTrim);
    }

    void SetTrim( uint8_t Index, Setup_Trim* Trim)
    {
      if( Index >= SIGNAL_TRIMS)
      {
        // That index is not available.
        return;
      }

      eeprom_write_block( Trim, &( Eeprom.Trim[ Index]), sizeof( Setup_Trim));
    }

    void GetTrim( uint8_t Index, Setup_Trim* Trim)
    {
      if( Index >= SIGNAL_TRIMS)
      {
        // That index is not available.
        return;
      }

      eeprom_read_block( Trim, &( Eeprom.Trim[ Index]), sizeof( Setup_Trim));
    }

    void ResetOutput( uint8_t Index)
    {
      Setup_Output DefaultOutput =
      {
        Index,

        SIGNAL_MAXIMUM_VALUE,
        SIGNAL_NEUTRAL_VALUE,
        SIGNAL_MINIMUM_VALUE,

        false
      };

      SetOutput( Index, &DefaultOutput);
    }

    void SetOutput( uint8_t Index, Setup_Output* Output)
    {
      if( Output >= OUTPUT_CHANNELS)
      {
        // That index is not available.
        return;
      }

      eeprom_write_block( Output, &( Eeprom.Output[ Index]), sizeof( Setup_Output));
    }

    void GetOutput( uint8_t Index, Setup_Output* Output)
    {
      if( Index >= OUTPUT_CHANNELS)
      {
        // That index is not available.
        return;
      }

      eeprom_read_block( Output, &( Eeprom.Output[ Index]), sizeof( Setup_Output));
    }

    void SetInvertedPPM( bool InvertedPPM)
    {
      eeprom_write_byte( &( Eeprom.InvertedPPM), InvertedPPM ? 1 : 0);
    }

    bool GetInvertedPPM( void)
    {
      uint8_t InvertedPPM = eeprom_read_byte( &( Eeprom.InvertedPPM));

      return( InvertedPPM == 1);
    }
};

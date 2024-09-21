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

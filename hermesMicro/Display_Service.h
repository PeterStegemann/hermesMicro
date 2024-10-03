// Copyright 2024 Peter Stegemann

#pragma once

#include "Display_U8x8.h"

#include <U8g2lib.h>               // Using U8G2 library "https://github.com/olikraus/u8g2"

class Display_Service
{
  private:
    u8g2_t display;

	public:
    Initialize( void)
    {
//      u8g2_Setup_sh1107_i2c_64x128_1
      u8g2_Setup_sh1106_i2c_128x64_noname_1
      (
        &display,
        U8G2_R0,
        Display_U8x8::Talk,
        Display_U8x8::Delay
      );

      u8x8_SetPin_HW_I2C( &( display.u8x8), U8X8_PIN_NONE);

      u8g2_InitDisplay( &display);
      u8g2_ClearDisplay( &display);
      u8g2_SetPowerSave( &display, 0);
    }

    u8g2_t* GetDisplay( void)
    {
      return( &display);
    }

    void SetFont( uint8_t* Font)
    {
      u8g2_SetFont( &display, Font);
      u8g2_SetFontRefHeightText( &display);
      u8g2_SetFontPosTop( &display);
    }
};

// Copyright 2024 Peter Stegemann

#pragma once

#include "U8x8_AVR.h"

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
        U8x8_AVR_TWI,
        u8x8_avr_delay
      );

      u8x8_SetPin_HW_I2C( &( display.u8x8), U8X8_PIN_NONE);

      u8g2_InitDisplay( &display);
      u8g2_ClearDisplay( &display);
      u8g2_SetPowerSave( &display, 0);
    }

    void Draw( void)
    {
      u8g2_FirstPage( &display);

      do
      {
//      u8g2_ClearBuffer(&display);
        u8g2_SetFont( &display, u8g2_font_smart_patrol_nbp_tr);
        u8g2_SetFontRefHeightText( &display);
        u8g2_SetFontPosTop( &display);
        u8g2_DrawStr( &display, 0, 0, "u8g2 AVR HW I2C");
//      u8g2_SendBuffer(&display);
      }
      while( u8g2_NextPage( &display));
    }
};

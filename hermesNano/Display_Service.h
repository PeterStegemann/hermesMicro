// Copyright 2024 Peter Stegemann

#pragma once

#include "Display_U8x8.h"

#include <U8g2lib.h>               // Using U8G2 library "https://github.com/olikraus/u8g2"

#include <stdio.h>

class Display_Service
{
  public:
    static const uint8_t GAUGE_SIZE = 7;

  private:
    u8g2_t display;

    void setFont( uint8_t* Font)
    {
      u8g2_SetFont( &display, Font);
      u8g2_SetFontRefHeightText( &display);
      u8g2_SetFontPosTop( &display);
    }

	public:
    Initialize( void)
    {
/*
      u8g2_Setup_sh1107_i2c_64x128_1
      (
        &display,
        U8G2_R1,
        Display_U8x8::Talk,
        Display_U8x8::Delay
      );
*/
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

    uint8_t GetWidth( void)
    {
      return( u8g2_GetDisplayWidth( &display));
    }

    uint8_t GetHeight( void)
    {
      return( u8g2_GetDisplayHeight( &display));
    }

    uint8_t GetHorizontalCenter( void)
    {
      return( GetWidth() / 2);
    }

    void FirstPage( void)
    {
        u8g2_FirstPage( &display);
    }

    bool NextPage( void)
    {
        return( u8g2_NextPage( &display));
    }

    void SetMiniFont( void)
    {
      setFont( u8g2_font_profont11_mr);
    }

    void SetSmallFont( void)
    {
      setFont( u8g2_font_profont17_mr);
    }

    void SetMediumFont( void)
    {
      setFont( u8g2_font_profont22_tr);
    }

    void SetLargeFont( void)
    {
      setFont( u8g2_font_profont29_tr);
    }

    void Print( uint8_t Left, uint8_t Top, const char* String)
    {
      u8g2_DrawStr( &display, Left, Top, String);
    }

    uint8_t GetWidth( const char* String)
    {
      return( u8g2_GetUTF8Width( &display, String));
    }

    uint8_t GetHorizontalCenter( const char* String)
    {
      return( GetHorizontalCenter() - GetWidth( String) / 2);
    }
 
    void PrintCentered( uint8_t Top, const char* String)
    {
      Print( GetHorizontalCenter( String), Top, String);
    }

    void PrintFormat( uint8_t Left, uint8_t Top, const char* Format, ...)
    {
      va_list Arguments;

      va_start( Arguments, Format);

      PrintFormatVAList( Left, Top, Format, &Arguments);

      va_end( Arguments);
    }

    void PrintFormatVAList( uint8_t Left, uint8_t Top, const char* Format, va_list* Arguments)
    {
      // Sorry, no more than 60 characters.
      char String[ 60];

      vsnprintf( String, sizeof( String), Format, *Arguments);

      Print( Left, Top, String);
    }

    void HorizontalFillGauge( uint8_t Left, uint8_t Top, uint8_t Size, uint8_t Value)
    {
      u8g2_DrawBox( &display, Left, Top, Value, GAUGE_SIZE);
      u8g2_DrawFrame( &display, Left + Value, Top, Size - Value, GAUGE_SIZE);
    }

    void HorizontalKnobGauge( uint8_t Left, uint8_t Top, uint8_t Size, uint8_t Value)
    {
      u8g2_DrawRFrame( &display, Left, Top + 1, Size, GAUGE_SIZE - 2, 1);
      u8g2_DrawBox( &display, Left + Value - 2, Top, 4, GAUGE_SIZE);
    }

    void VerticalKnobGauge( uint8_t Left, uint8_t Top, uint8_t Size, uint8_t Value)
    {
      u8g2_DrawRFrame( &display, Left + 1, Top, GAUGE_SIZE - 2, Size, 1);
      u8g2_DrawBox( &display, Left, Top + Value - 2, GAUGE_SIZE, 4);
    }
};

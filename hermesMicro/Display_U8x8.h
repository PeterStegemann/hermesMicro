// Copyright 2024 Peter Stegemann
//
// Based on code from U8G2 library

#pragma once

#include "TWI.h"

#include <U8g2lib.h>               // Using U8G2 library "https://github.com/olikraus/u8g2"

class Display_U8x8
{
  public:
    static uint8_t Talk( u8x8_t* u8x8, uint8_t Message, uint8_t ArgumentCount, void* Arguments)
    {
      uint8_t* Data;

      switch( Message)
      {
        case U8X8_MSG_BYTE_SEND :
        {
          Data = ( uint8_t*) Arguments;

          while( ArgumentCount--)
          {
            TWI_Write( *Data);

            Data++;
          }
        }
        break;

        case U8X8_MSG_BYTE_INIT :
        {
          TWI_Initialize();
        }
        break;

        case U8X8_MSG_BYTE_SET_DC : break;

        case U8X8_MSG_BYTE_START_TRANSFER :
        {
          TWI_OpenForWrite( u8x8_GetI2CAddress( u8x8));
        }
        break;

        case U8X8_MSG_BYTE_END_TRANSFER :
        {
          TWI_Stop();
        }
        break;

        default : return( 0);
      }

      return( 1);
    }

    static uint8_t Delay( u8x8_t * u8x8, uint8_t Message, uint8_t ArgumentCount, void *Arguments)
    {
      switch( Message)
      {
        case U8X8_MSG_DELAY_NANO :          // delay ArgumentCount * 1 nano second
        {
          // At 20Mhz, each cycle is 50ns, the call itself is slower.
        }
        break;

        case U8X8_MSG_DELAY_100NANO :       // delay ArgumentCount * 100 nano seconds
        {
          #define NANOS_PER_CLOCK     ( 1000000000UL / F_CPU)
          // Approximate best case values...
          #define CALL_CYCLES         26UL
          #define CALC_CYCLES         4UL
          #define RETURN_CYCLES       4UL
          #define CYCLES_PER_LOOP     4UL

          uint8_t Cycles  = ( 100UL * ArgumentCount) / ( NANOS_PER_CLOCK * CYCLES_PER_LOOP);

          if( Cycles > CALL_CYCLES + RETURN_CYCLES + CALC_CYCLES)
          {
            break;
          }

          __asm__ __volatile__
          (
            "1: sbiw %0,1" "\n\t"  // 2 cycles
            "brne 1b":"=w" ( Cycles):"0" ( Cycles)  // 2 cycles
          );
        }
        break;

        case U8X8_MSG_DELAY_10MICRO :       // delay ArgumentCount * 10 micro seconds
        {
          while( ArgumentCount--)
          {
            _delay_us( 10);
          }
        }
        break;

        case U8X8_MSG_DELAY_MILLI :         // delay ArgumentCount * 1 milli second
        {
          Utility::Pause( ArgumentCount);
        }
        break;

        default :
        {
          return( 0);
        }
      }

      return( 1);
    }
};

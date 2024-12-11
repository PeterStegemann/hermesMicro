// Copyright 2007 Peter Stegemann

#pragma once

#include "Types.h"

#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <util/delay.h>

#include <stdlib.h> 
#include <string.h>

#define Utility_BitValue( Bit)				  ( 1 << ( Bit))
#define Utility_SetBit( Value, Bit)			(( Value) |= Utility_BitValue( Bit))
#define Utility_ClearBit( Value, Bit)		(( Value) &= ~Utility_BitValue( Bit))
#define Utility_GetBit( Value, Bit)			(( Value) & Utility_BitValue( Bit))

#define Utility_Max( A, B)              ((( A) >= ( B)) ? ( A) : ( B))
#define Utility_Min( A, B)              ((( A) <= ( B)) ? ( A) : ( B))
#define Utility_Abs( Value)             (( Value < 0) ? -( Value) : ( Value))

class Utility
{
	public:
		// Wait some milliseconds. Internal delay can't wait very long.
		static void Pause( uint16_t Milliseconds)
    {
      while( Milliseconds > 0)
      {
        _delay_ms( 1);

        Milliseconds--;
      }
    }

		// Quick soft reset.
		static void SoftReset( void)
    {
      // Jump into reset vector.
      goto *(( void**) NULL);
    }

		// Slow hard rest.
		static void HardReset( void)
    {
      // Stop all interrupt activity.
      cli();

      // Enable watchdog.
      wdt_enable( WDTO_1S);

      while( true);
    }

		// Convert a string to boolean.
		static bool String2Boolean( const char* String)
    {
      if( !strcasecmp( String, "On") || !strcasecmp( String, "True"))
      {
        return( true);
      }
      else
      {
        return( false);
      }
    }
/*
void Utility::PrintByteBits( LCD* UseLCD, uint16_t Left, uint16_t Bottom, const char* Text,
	                         uint8_t Value)
{
	char BitString[] = "00000000";

	uint8_t ValueCopy = Value;

	for( uint8_t Count = 0; Count < 8; Count++)
	{
		if( ValueCopy & 0x01)
		{
			BitString[ 7 - Count] = '1';
		}
		else
		{
			BitString[ 7 - Count] = '0';
		}

		ValueCopy = ValueCopy >> 1;		
	}

 	UseLCD->PrintFormat_P( Left, Bottom, FONT::FID_Mini, LCD::WarmYellow, LCD::Black,
						   LCD::PO_Proportional, "%s%s", Text, BitString);
}
*/
};

void* operator new( size_t Size)
{
	return( NULL);
} 

void operator delete( void* Buffer)
{
}

extern "C" void __cxa_pure_virtual( void)
{
}

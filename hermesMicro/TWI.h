// Copyright 2024 Peter Stegemann
//
// Based on code from U8G2 library

#pragma once

#include <util/twi.h>

// TWI clock in Hz
#define TWI_CLOCK         100000L

/* TWI timer max delay */
#define TWI_TIMER_DELAY   0xFF

static uint16_t TWI_WaitTransmissionCompleted( void)
{
  uint16_t Timer = TWI_TIMER_DELAY;

  while( !( TWCR & ( 1 << TWINT)) && Timer--);

  return( Timer);
}

static uint16_t TWI_WaitStopped( void)
{
	uint16_t Timer = TWI_TIMER_DELAY;

	while(( TWCR & ( 1 << TWSTO)) && Timer--);

  return( Timer);
}

static uint8_t TWI_Result( void)
{
  // Get value of status register. Mask prescaler bits.
  return( TW_STATUS & 0xF8);
}

static void TWI_Initialize( void)
{
  // Initialize TWI clock: 100 kHz clock, TWPS = 0 => prescaler = 1

  TWSR = 0;                                 // No prescaler
  TWBR = (( F_CPU / TWI_CLOCK) - 16) / 2;   // Must be > 10 for stable operation
}

static void TWI_OpenForWrite( unsigned char address)
{
	uint8_t Result;

  while( true)
  {
    // Send start condition
    TWCR = ( 1 << TWINT) | ( 1 << TWSTA) | ( 1 << TWEN);

    TWI_WaitTransmissionCompleted();

    Result = TWI_Result();

    if(( Result != TW_START) && ( Result != TW_REP_START))
    {
      continue;
    }

    // Send device address
    TWDR = address;
    TWCR = ( 1 << TWINT) | ( 1 << TWEN);

    TWI_WaitTransmissionCompleted();

    Result = TWI_Result();

    if(( Result == TW_MT_SLA_NACK)||( Result == TW_MR_DATA_NACK)) 
    {
      // Device busy, send stop condition to terminate write operation
      TWCR = ( 1 << TWINT) | ( 1 << TWEN) | ( 1 << TWSTO);

      TWI_WaitStopped();

      continue;
    }

    break;
  }
}

static unsigned char TWI_Write( unsigned char Data)
{
  // Send data to the previously addressed device
	TWDR = Data;
	TWCR = ( 1 << TWINT) | ( 1 << TWEN);

	if( TWI_WaitTransmissionCompleted() == 0)
  {
		return( 1);
  }

  uint8_t Result = TWI_Result();

	if( Result != TW_MT_DATA_ACK)
  {
    return 1;
  }

  return( 0);
}

static void TWI_Stop( void)
{
  // Send stop condition
	TWCR = ( 1 << TWINT) | ( 1 << TWEN) | ( 1 << TWSTO);

  TWI_WaitStopped();
}

// Copyright 2008 Peter Stegemann

#pragma once

#include "Setup_Service.h"
#include "Signal.h"
#include "Types.h"
#include "Utility.h"

#include <avr/io.h>

#define SPEKTRUM_CHANNELS           6
#define SPEKTRUM_CHANNEL_BYTES      2

#define SPEKTRUM_SIGNAL_NEUTRAL     0x0200
#define SPEKTRUM_SIGNAL_RANGE       0x03FF

/*
 00 00 00 xx xx xx xx xx - ch1 0000-03FF
 
 00 00 01 xx xx xx xx xx - ch2 0400-07FF
 
 00 00 10 xx xx xx xx xx - ch3 0800-0BFF
 
 00 00 11 xx xx xx xx xx - ch4 0C00-0FFF
 
 00 01 00 xx xx xx xx xx - ch5 1000-13FF
 
 00 01 01 xx xx xx xx xx - ch6 1400-17FF
 */

enum SpektrumMode
{
	SM_BIND               = 0x80,
	SM_RANGE_TEST         = 0x20,
	SM_COUNTRY_NORMAL     = 0x00,
	SM_COUNTRY_FRANCE     = 0x10,
	SM_DSM2               = 0x00,
	SM_DSMX               = 0x08
};

extern class Output_Spektrum_Engine* Output_Spektrum_Singleton;

class Output_Spektrum_Engine
{
	private:
    Setup_Service* setupService;

		struct __attribute__(( __packed__)) Channel
		{
			uint8_t Data[ SPEKTRUM_CHANNEL_BYTES];
		};

		struct __attribute__(( __packed__)) Frame
		{
			uint8_t Mode;
			uint8_t Reserved;

			Channel Channels[ SPEKTRUM_CHANNELS];
		};

	private:
		// This array holds all channel values that have to be transformed and copied into the
		// data frame.
		volatile Channel channels[ SPEKTRUM_CHANNELS];

		// This has to be set to true when the channels are ready to be used by the interrupt
		// routine. When the routine is done with the data, it sets the flag back to false, so new
		// values can be set.
		volatile bool channelsValid;
		// This holds the frame that is sent out.
		volatile Frame data;

		volatile uint16_t currentByte;

		// Try to copy a new set of channels if it is available.
		void copyChannels( void)
    {
      if( channelsValid != true)
      {
        // No valid data set yet.
        return;
      }

      data.Mode &= ~SM_BIND;

      for( uint8_t Count = 0; Count < SPEKTRUM_CHANNELS; Count++)
      {
        data.Channels[ Count].Data[ 0] = channels[ Count].Data[ 0];
        data.Channels[ Count].Data[ 1] = channels[ Count].Data[ 1];
      }

      channelsValid = false;
    }

	public:
		Output_Spektrum_Engine( Setup_Service* SetupService)
      : setupService( SetupService)
      , channelsValid( false)
    {
      Output_Spektrum_Singleton = this;

      // Reset all channels.
      for( uint8_t Count = 0; Count < SPEKTRUM_CHANNELS; Count++)
      {
        channels[ Count].Data[ 0] = Count << 2;
        channels[ Count].Data[ 1] = 0x00;
      }

      data.Mode = SM_DSM2;
      data.Reserved = 0;

      // Reset all signals.
      for( uint8_t Count = 0; Count < SPEKTRUM_CHANNELS; Count++)
      {
        data.Channels[ Count].Data[ 0] = Count << 2;
        data.Channels[ Count].Data[ 1] = 0x00;
      }
    }

		// This is for the interrupt, not for you.
		void Process( void)
    {
      volatile uint8_t* Data = reinterpret_cast< volatile uint8_t*>( &data);

      UDR0 = Data[ currentByte++];

      if( currentByte == sizeof( data))
      {
        currentByte = 0;

        copyChannels();

        Utility_ClearBit( UCSR0B, UDRIE0);
      }
    }

		// Start sending loop.
		void Start( bool BindMode)
    {
      if( BindMode == true)
      {
        data.Mode |= SM_BIND;
      }

      currentByte = 0;

      // Set baud rate
      //
      // UBRR = ( Frequency / 16 * Baudrate) - 1
      //
      // ( 16000000 / ( 16 * 125000)) - 1 = 7
      // (  8000000 / ( 16 * 125000)) - 1 = 3
      UBRR0 = 7;

      UCSR0A = ( 1 << U2X0);

      // Transmit buffer empty interrupt, Enable sending, 8 bit data.
      UCSR0B = ( 0 << RXCIE0) | ( 0 << TXCIE0) | ( 1 << UDRIE0) | ( 0 << RXEN0) | ( 1 << TXEN0) |
      ( 0 << UCSZ02);

      // Asynchronous, No parity, 1 stop bit, 8 bit data
      UCSR0C = ( 0 << UMSEL01) | ( 0 << UMSEL00) | ( 0 << UPM01) | ( 0 << UPM00) | ( 0 << USBS0) |
      ( 1 << UCSZ01) | ( 1 << UCSZ00);
    }

		// Set a specific channel to a new value. This one waits until the sending routine has
		// copied the last value set.
		void SetChannel( uint8_t ChannelId, int16_t Value)
    {
      if( ChannelId >= SPEKTRUM_CHANNELS)
      {
        // Invalid channel!
        return;
      }

      // Clip to valid values.
      if( Value > SIGNAL_MAXIMUM_VALUE)
      {
        Value = SIGNAL_MAXIMUM_VALUE;
      }
      else if( Value < SIGNAL_MINIMUM_VALUE)
      {
        Value = SIGNAL_MINIMUM_VALUE;
      }

      int32_t SignalValue = Value;
      SignalValue *= SPEKTRUM_SIGNAL_RANGE;
      SignalValue /= SIGNAL_VALUE_RANGE;
      SignalValue += SPEKTRUM_SIGNAL_NEUTRAL;

      // Wait for the interrupt routine to be ready with fetching the last data set.
      while( ReadyForData() == false)
      {
        Utility::Pause( 1);
      }

      channels[ ChannelId].Data[ 0] = ( ChannelId << 2) + (( SignalValue & 0x0300) >> 8);
      channels[ ChannelId].Data[ 1] = SignalValue & 0x00ff;
    }

		// Marks the data as beeing complete, with the next cycle these values will be used.
		void SetChannelsValid( void)
    {
      channelsValid = true;

      Utility_SetBit( UCSR0B, UDRIE0);
    }

		// Returns false when engine hasn't processed the last dataset yet. In that case, calls to
		// SetChannel() would block.
		bool ReadyForData( void)
    {
      // As long as the old data was not picked up, we're not ready for new data.
      return( channelsValid != true);
    }
};

static Output_Spektrum_Engine* Output_Spektrum_Singleton;

ISR( USART_UDRE_vect)
{
  Output_Spektrum_Singleton->Process();
}

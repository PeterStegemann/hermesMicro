// Copyright 2010 Peter Stegemann

#pragma once

#include "Setup_Service.h"
#include "Signal.h"
#include "System_Ports.h"
#include "Types.h"
#include "Utility.h"

#include <avr/io.h>
#include <util/delay.h>

#define PPM_CHANNELS          8
#define PPM_SIGNALS           PPM_CHANNELS + 1

// We're using the /8 clock divider.
#define PPM_MS_CLOCK_FACTOR   ( F_CPU / 100) / 800

// 22.5ms
#define PPM_SIGNAL_FRAME      ((( uint32_t) 225) * PPM_MS_CLOCK_FACTOR)
//  0.3ms
#define PPM_SIGNAL_PAUSE      ((( uint32_t)   3) * PPM_MS_CLOCK_FACTOR)
//  1.2ms
#define PPM_SIGNAL_NEUTRAL    ((( uint32_t)  12) * PPM_MS_CLOCK_FACTOR)
//  1.0ms
#define PPM_SIGNAL_RANGE      ((( uint32_t)  10) * PPM_MS_CLOCK_FACTOR)

// Standard sync if all channels are neutral.
#define PPM_SYNC_TIME         PPM_SIGNAL_FRAME - (( PPM_SIGNAL_PAUSE + PPM_SIGNAL_NEUTRAL) * PPM_CHANNELS)

extern class Output_Ppm_Engine* Output_Ppm_Singleton;

class Output_Ppm_Engine
{
	private:
    Setup_Service* setupService;

		// Operate in inverted mode?
		volatile bool inverted;

		// This array holds all channel values that have to be transformed and copied into the
		// signals array.
		volatile uint16_t channels[ PPM_CHANNELS];
		// This has to be set to true when the channels are ready to be used by the interrupt
		// routine. When the routine is done with the data, it sets the flag back to false, so new
		// values can be set.
		volatile bool channelsValid;
		// This array holds all signals that are sent out. First is the start signal.
		uint16_t signals[ PPM_SIGNALS];

		// Which one we're currently sending.
		uint8_t currentSignal;
		// Here we remember the next signal state.
		bool signalState;

		// Try to copy a new set of channels if it is available.
		void copyChannels( void)
    {
      if( channelsValid != true)
      {
        // No valid data set yet.
        return;
      }

      uint32_t TotalSignalTime = PPM_SIGNAL_PAUSE * PPM_CHANNELS;

      for( uint8_t Count = 0; Count < PPM_CHANNELS; Count++)
      {
        signals[ Count + 1] = channels[ Count];
        TotalSignalTime += channels[ Count];
      }

      // Sync time is the first signal.
      signals[ 0] = PPM_SIGNAL_FRAME - TotalSignalTime;

      channelsValid = false;
    }

	public:
		Output_Ppm_Engine( Setup_Service* SetupService)
      : inverted( false)
      , channelsValid( false)
    {
      setupService = SetupService;

      Output_Ppm_Singleton = this;

      // Reset all channels.
      for( uint8_t Count = 0; Count < PPM_CHANNELS; Count++)
      {
        channels[ Count] = PPM_SIGNAL_NEUTRAL;
      }

      // Reset all signals.
      signals[ 0] = PPM_SYNC_TIME;

      for( uint8_t Count = 1; Count < PPM_SIGNALS; Count++)
      {
        signals[ Count] = PPM_SIGNAL_NEUTRAL;
      }
    }

		// This is for the interrupt, not for you.
		void Process( void)
    {
      // Every high signal/start phase is followed by a low pause.
      if( signalState == 1)
      {
        // Set signal width for current signal. 
        OCR1A = signals[ currentSignal];

        if( inverted)
        {
          // Switch to 0 after this one.
          PPM_TIMERA = BIT_VALUE( COM1A1);
        }
        else
        {
          // Switch to 1 after this one.
          PPM_TIMERA = BIT_VALUE( COM1A1) | BIT_VALUE( COM1A0);
        }

        signalState = 0;

        // Skip to next signal.
        currentSignal++;

        if( currentSignal == PPM_SIGNALS)
        {
          currentSignal = 0;

          copyChannels();
        }
      }
      else
      {
        // Set pause time.
        OCR1A = PPM_SIGNAL_PAUSE;

        if( inverted == true)
        {
          // Switch to 1 after this one.
          PPM_TIMERA = BIT_VALUE( COM1A1) | BIT_VALUE( COM1A0);
        }
        else
        {
          // Switch to 0 after this one.
          PPM_TIMERA = BIT_VALUE( COM1A1);
        }

        signalState = 1;
      }
    }

		// Start sending loop.
		void Start( bool BindMode)
    {
      inverted = setupService->GetInvertedPPM();

      // We will start with the start signal.
      currentSignal = 0;

      // Clear counter before use. This will also clear all other settings.
      TCNT1 = 0;
      // Set some lead in time before the first match.
      OCR1A = PPM_SIGNAL_FRAME;
      // OCR1A will use PB1 as output, so set it up.
      PPM_DDR |= BIT_VALUE( PPM_SIGNAL);

      if( inverted)
      {
        // Switch to 1  when matching.
        PPM_TIMERA = BIT_VALUE( COM1A1) | BIT_VALUE( COM1A0);
      }
      else
      {
        // Switch to 0 when matching.
        PPM_TIMERA = BIT_VALUE( COM1A1);
      }

      signalState = 1;
      // Clear timer by match and split clock for timer by 8.
      TCCR1B = BIT_VALUE( WGM12) | BIT_VALUE( CS11);
      // Switch interrupt for compare match on.
      TIMSK1 = BIT_VALUE( OCIE1A);
    }

		// Set PPM inverted/normal.
		void SetInvertedPPM( bool Inverted)
    {
      inverted = Inverted;
    }

		// Set a specific channel to a new value. This one waits until the sending routine has
		// copied the last value set.
		void SetChannel( uint8_t ChannelId, int16_t Value)
    {
      if( ChannelId >= PPM_CHANNELS)
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

      int32_t SignalValue = Value * PPM_SIGNAL_RANGE;
      SignalValue /= SIGNAL_VALUE_RANGE;
      SignalValue += PPM_SIGNAL_NEUTRAL;

      // Wait for the interrupt routine to be ready with fetching the last data set.
      while( ReadyForData() == false)
      {
        Utility::Pause( 1);
      }

      channels[ ChannelId] = SignalValue;
    }

		// Marks the data as beeing complete, with the next cycle these values will be used.
    void SetChannelsValid( void)
    {
      channelsValid = true;
    }

		// Returns false when engine hasn't processed the last dataset yet. In that case, calls to
		// SetChannel() would block.
    bool ReadyForData( void)
    {
      // As long as the old data was not picked up, we're not ready for new data.
      return( channelsValid != true);
    }
};

static Output_Ppm_Engine* Output_Ppm_Singleton;

ISR( TIMER1_COMPA_vect)
{
  Output_Ppm_Singleton->Process();
}

// Copyright 2009 Peter Stegemann

#pragma once

#include "Input_Service.h"
#include "Signal_Processor.h"
#include "Status_Engine.h"
#include "Store_Processor.h"
#include "Types.h"

#include <avr/interrupt.h>

#define FAST_INTERRUPT_TICKS_PER_SECOND      1000
#define FAST_INTERRUPT_CLOCK_FACTOR          ( F_CPU / FAST_INTERRUPT_TICKS_PER_SECOND) / 64

#define SLOW_INTERRUPT_TICKS_PER_SECOND      100
#define SLOW_INTERRUPT_CLOCK_FACTOR          ( F_CPU / SLOW_INTERRUPT_TICKS_PER_SECOND) / 64

extern class Interrupt_Service* Interrupt_Singleton;

class Interrupt_Service
{
	private:
    Input_Service* inputService;
    Signal_Processor* signalProcessor;
    Status_Engine* statusEngine;
    Store_Processor* storeProcessor;

		volatile uint16_t timeMillis;
		volatile uint16_t timeSeconds;

	public:
    Interrupt_Service
    (
      Input_Service* InputService,
      Signal_Processor* SignalProcessor,
      Status_Engine* StatusEngine,
      Store_Processor* StoreProcessor
    )
      : inputService( InputService)
      , signalProcessor( SignalProcessor)
      , statusEngine( StatusEngine)
      , storeProcessor( StoreProcessor)
      , timeMillis( 0)
      , timeSeconds( 0)
    {
      Interrupt_Singleton = this;
    }

		// Start processing loop.
		void Start( void)
    {
      // Clear counter before use. This will also clear all other settings.
      TCNT0 = 0;
      // Wakeup often.
      OCR0A = FAST_INTERRUPT_CLOCK_FACTOR;
      // Wakeup less often.
 //     OCR0B = SLOW_INTERRUPT_CLOCK_FACTOR;
      // Clear timer by match and split clock for timer by 64.
      TCCR0A = Utility_BitValue( WGM01);
      TCCR0B = Utility_BitValue( CS01) | Utility_BitValue( CS00);
      // Switch interrupt for compare match on.
      TIMSK0 = Utility_BitValue( OCIE0A) | Utility_BitValue( OCIE0B);
/*
      // Clear counter before use. This will also clear all other settings.
      TCNT2 = 0;
      // Wakeup often.
      OCR2A = SLOW_INTERRUPT_CLOCK_FACTOR;
      // Clear timer by match and split clock for timer by 64.
      TCCR2A = Utility_BitValue( WGM21);
      TCCR2B = Utility_BitValue( CS21) | Utility_BitValue( CS20);
      // Switch interrupt for compare match on.
      TIMSK2 = Utility_BitValue( OCIE2A);
*/
    }

		// This is for the interrupt, not for you.
		void ProcessFast( void)
    {
      // Time calculation is _always_ done.
      timeMillis++;

      // Second?
      if( timeMillis == 1000)
      {
        timeSeconds++;
        timeMillis = 0;
      }

      statusEngine->Process();
      inputService->Process();
    }

		// This is for the interrupt, not for you.
		void ProcessSlow( void)
    {
      signalProcessor->Process();
    }

    void GetTime( uint16_t* Millis, uint16_t* Seconds)
    {
      if( Millis != NULL)
      {
        *Millis = timeMillis;
      }

      if( Seconds != NULL)
      {
        *Seconds = timeSeconds;
      }
    }
};

static Interrupt_Service* Interrupt_Singleton;

ISR( TIMER0_COMPA_vect, ISR_NOBLOCK)
{
	Interrupt_Singleton->ProcessFast();
}

ISR( TIMER0_COMPB_vect, ISR_NOBLOCK)
{
	Interrupt_Singleton->ProcessSlow();
}

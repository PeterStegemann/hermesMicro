// Copyright 2009 Peter Stegemann

#pragma once

#include "Input_Service.h"
#include "Signal_Processor.h"
#include "Status_Engine.h"
#include "Store_Processor.h"
#include "Types.h"

#include <avr/interrupt.h>

#define INTERRUPT_TICKS_PER_SECOND      1000
#define INTERRUPT_TICKS_PER_MS          INTERRUPT_TICKS_PER_SECOND / 1000
#define INTERRUPT_CLOCK_FACTOR          ( F_CPU / INTERRUPT_TICKS_PER_SECOND) / 64

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
    {
    }

		// Start processing loop.
		void Start( void)
    {
      timeMillis = 0;
      timeSeconds = 0;

      Interrupt_Singleton = this;

      // Clear counter before use. This will also clear all other settings.
      TCNT0 = 0;
      // Wakeup often.
      OCR0A = INTERRUPT_CLOCK_FACTOR;
      // Clear timer by match and split clock for timer by 64.
      INPUT_TIMERA = BIT_VALUE( WGM01);
      INPUT_TIMERB = BIT_VALUE( CS01) | BIT_VALUE( CS00);
      // Switch interrupt for compare match on.
      TIMSK0 = BIT_VALUE( OCIE0A);
    }

		// This is for the interrupt, not for you.
		void Process( void)
    {
      // Time calculation is _always_ done.
      timeMillis++;

      // Second?
      if( timeMillis == 1000)
      {
        timeSeconds++;
        timeMillis = 0;
      }

      int32_t value = timeSeconds;
      value *= 1000;
      value += timeMillis;
      value *= 10;

      if(( value % ( 1000 / 45)) == 0)
      {
        //		spektrumEngine.SetChannelsValid();
        signalProcessor->Process();
      }

      statusEngine->Process();
      inputService->Process();
      //	signalProcessor.Process();
    }
};

static Interrupt_Service* Interrupt_Singleton;

ISR( TIMER0_COMPA_vect, ISR_NOBLOCK)
{
	Interrupt_Singleton->Process();
}

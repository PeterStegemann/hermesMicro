// Copyright 2008 Peter Stegemann

#pragma once

#include "Ports.h"
#include "Types.h"
#include "Utility.h"

#define STATUS_BLINK_OFF            2000
#define STATUS_BLINK_ON             STATUS_BLINK_OFF + 100

#define STATUS_BLINK_OFF_FAST       200
#define STATUS_BLINK_ON_FAST        STATUS_BLINK_OFF_FAST + 100

#define STATUS_BLINK_OFF_SUPER_FAST 50
#define STATUS_BLINK_ON_SUPER_FAST  STATUS_BLINK_OFF_FAST + 25

#define STATUS_LONG_BEEP_LENGTH     100
#define STATUS_BEEP_LENGTH          20
#define STATUS_TICK_LENGTH          2

#if defined( HAS_STATUS_LED)
  #define STATUS_LED_OFF( LED)        BIT_CLEAR( STATUS_PORT, LED);
  #define STATUS_LED_ON( LED)         BIT_SET( STATUS_PORT, LED);
#endif

class Status_Engine
{
	public:
		enum LEDState
		{
			LED_OFF,
			LED_BLINK,
			LED_BLINK_FAST,
			LED_BLINK_SUPER_FAST,
			LED_ON
		};

	private:
		LEDState ledState;

		// Blink count.
		uint16_t ledBlinkCount;
		uint16_t beepCount;

	public:
		Status_Engine( void)
      : ledState( LED_OFF)
      , ledBlinkCount( 0)
      , beepCount( 0)
    {
    }

		// Initialize status engine.
    void Initialize( void)
    {
      #if defined( HAS_STATUS_LED)
        BIT_SET( STATUS_DDR, STATUS_LED);
        STATUS_LED_OFF( STATUS_LED);
      #endif

      BIT_SET( BUZZER_DDR, BUZZER_NOISE);
      BIT_CLEAR( BUZZER_PORT, BUZZER_NOISE);
    }

		// This is for the interrupt, not for you.
		void Process( void)
    {
      #if defined( HAS_STATUS_LED)
        // Set status LED.
        switch( ledState)
        {
          case LED_OFF :
          {
            STATUS_LED_OFF( STATUS_LED);
          }
          break;

          case LED_BLINK :
          {
            ledBlinkCount++;

            if( ledBlinkCount == STATUS_BLINK_OFF)
            {
              STATUS_LED_OFF( STATUS_LED);
            }
            else if( ledBlinkCount == STATUS_BLINK_ON)
            {
              STATUS_LED_ON( STATUS_LED);

              ledBlinkCount = 0;
            }
          }
          break;

          case LED_BLINK_FAST :
          {
            ledBlinkCount++;

            if( ledBlinkCount == STATUS_BLINK_OFF_FAST)
            {
              STATUS_LED_OFF( STATUS_LED);
            }
            else if( ledBlinkCount == STATUS_BLINK_ON_FAST)
            {
              STATUS_LED_ON( STATUS_LED);

              ledBlinkCount = 0;
            }
          }
          break;

          case LED_BLINK_SUPER_FAST :
          {
            ledBlinkCount++;

            if( ledBlinkCount == STATUS_BLINK_OFF_SUPER_FAST)
            {
              STATUS_LED_OFF( STATUS_LED);
            }
            else if( ledBlinkCount == STATUS_BLINK_ON_SUPER_FAST)
            {
              STATUS_LED_ON( STATUS_LED);

              ledBlinkCount = 0;
            }
          }
          break;

          case LED_ON :
          {
            STATUS_LED_ON( STATUS_LED);
          }
          break;
        }
      #endif

      // Count down beep.
      if( beepCount > 0)
      {
        beepCount--;

        if( beepCount == 0)
        {
          BIT_CLEAR( BUZZER_PORT, BUZZER_NOISE);
        }
      }
    }

		// Set LED state.
		void SetLedState( LEDState UseLEDState)
    {
      ledState = UseLEDState;
      ledBlinkCount = 0;
    }

		// Give a long beep.
    void LongBeep( void)
    {
      beepCount = STATUS_LONG_BEEP_LENGTH;

      BIT_SET( BUZZER_PORT, BUZZER_NOISE);
    }

		// Give a beep.
    void Beep( void)
    {
      beepCount = STATUS_BEEP_LENGTH;
      
      BIT_SET( BUZZER_PORT, BUZZER_NOISE);
    }

		// Give a tick.
    void Tick( void)
    {
      beepCount = STATUS_TICK_LENGTH;

      BIT_SET( BUZZER_PORT, BUZZER_NOISE);
    }
};

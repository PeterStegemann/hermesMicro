// Copyright 2007 Peter Stegemann

#pragma once

#include "Input_Defines.h"
#include "Ports.h"
#include "Types.h"
#include "Utility.h"

class Input_Processor
{
	private:
		bool buttonCurrent[ INPUT_BUTTONS];
		volatile uint8_t buttonCycles[ INPUT_BUTTONS];

		bool switchCurrent[ INPUT_SWITCHES];

	public:
		// Initialize.
		void Initialize( void)
    {
      for( uint8_t Count = 0; Count < INPUT_BUTTONS; Count++)
      {
        buttonCurrent[ Count] = false;
        buttonCycles[ Count] = 0;
      }

      for( uint8_t Count = 0; Count < INPUT_SWITCHES; Count++)
      {
        switchCurrent[ Count] = false;
      }

      // Input.
      INPUT_TRIM_DDR &= ~INPUT_BUTTON_INPUT_PINS;
      // Pullups.
      INPUT_TRIM_PORT |= INPUT_BUTTON_INPUT_PINS;

      // Output
      INPUT_TRIM_DDR |= INPUT_BUTTON_OUTPUT_PINS;
      // Default on.
      INPUT_TRIM_PORT |= INPUT_BUTTON_OUTPUT_PINS;

      // Input.
      INPUT_INVERT_DDR &= ~INPUT_SWITCH_PINS;
      // Pullups.
      INPUT_INVERT_PORT |= INPUT_SWITCH_PINS;
    }

		// This is for the interrupt, not for you.
    void Process( void)
    {
      // Go trough trim buttons.
      uint8_t ButtonId = 0;

      for( uint8_t OutputId = INPUT_TRIM_OUTPUT_0; OutputId <= INPUT_TRIM_OUTPUT_1; OutputId++)
      {
        // Pull down.
        BIT_CLEAR( INPUT_TRIM_PORT, OutputId);

        for( uint8_t InputId = INPUT_TRIM_INPUT_0; InputId <= INPUT_TRIM_INPUT_3; InputId++)
        {
          bool ButtonPressed = ( BIT_CHECK( INPUT_TRIM_PIN, InputId) == false);

          // React on state change.
          if( ButtonPressed != buttonCurrent[ ButtonId])
          {
            if( ButtonPressed == false)
            {
              // React on button beeing released;
              buttonCycles[ ButtonId]++;
    //					statusEngine.Tick();
            }

            // Remind last state.
            buttonCurrent[ ButtonId] = ButtonPressed;
          }

          ButtonId++;
        }

        // Pull up.
        BIT_SET( INPUT_TRIM_PORT, OutputId);
      }

      // Go trough invert switches.
      uint8_t SwitchId = 0;

      for( uint8_t InputId = INPUT_INVERT_ANALOG_A; InputId <= INPUT_INVERT_ANALOG_C; InputId++)
      {
        switchCurrent[ SwitchId] = ( BIT_CHECK( INPUT_INVERT_PIN, InputId) == false);

        SwitchId++;
      }
    }

		// Get current value of the button.
		// Cycles press & release cycles since last call
		// Current current button state
		void GetButton( uint8_t ButtonId, uint8_t* Cycles = NULL, bool* Current = NULL)
    {
      if( Cycles != NULL)
      {
        *Cycles = buttonCycles[ ButtonId];
        buttonCycles[ ButtonId] = 0;
      }

      if( Current != NULL)
      {
        *Current = buttonCurrent[ ButtonId];
      }
    }

		// Get current value of the switch.
		void GetSwitch( uint8_t SwitchId, bool* Current)
    {
      if( Current != NULL)
      {
        *Current = switchCurrent[ SwitchId];
      }
    }
};

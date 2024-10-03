// Copyright 2007 Peter Stegemann

#pragma once

#include "Input_Rotary.h"
#include "System_Ports.h"
#include "Types.h"
#include "Utility.h"

#define INPUT_BUTTONS               INPUT_DIGITAL_PORTS

#define INPUT_BUTTON_PINS		        ( BIT_VALUE( INPUT_DIGITAL_A) | \
                                      BIT_VALUE( INPUT_DIGITAL_B) | \
                                      BIT_VALUE( INPUT_DIGITAL_C) | \
                                      BIT_VALUE( INPUT_DIGITAL_D) | \
                                      BIT_VALUE( INPUT_DIGITAL_E) | \
                                      BIT_VALUE( INPUT_DIGITAL_F) | \
                                      BIT_VALUE( INPUT_DIGITAL_G) | \
                                      BIT_VALUE( INPUT_DIGITAL_H))

#define INPUT_MENU_PINS             ( BIT_VALUE( INPUT_MENU_ROTARY_A) | \
          					      		        BIT_VALUE( INPUT_MENU_ROTARY_B) | \
					          		              BIT_VALUE( INPUT_MENU_ROTARY_BUTTON))

class Input_Service
{
	private:
		bool buttonCurrent[ INPUT_BUTTONS];
		volatile uint8_t buttonCycles[ INPUT_BUTTONS];

  	Input_Rotary rotary;
  	volatile int8_t rotarySelect;

  	bool lastRotaryButton;
	  volatile uint8_t rotaryButton;

	public:
		// Initialize.
		void Initialize( void)
    {
      for( uint8_t Count = 0; Count < INPUT_BUTTONS; Count++)
      {
        buttonCurrent[ Count] = false;
        buttonCycles[ Count] = 0;
      }

      // Input.
      INPUT_DIGITAL_DDR &= ~INPUT_BUTTON_PINS;
      // Pullups.
      INPUT_DIGITAL_PORT |= INPUT_BUTTON_PINS;
    }

		// This is for the interrupt, not for you.
    void Process( void)
    {
      // Go trough buttons.
      uint8_t ButtonId = 0;

      for( uint8_t InputId = INPUT_DIGITAL_A; InputId <= INPUT_DIGITAL_H; InputId++)
      {
        bool ButtonPressed = ( BIT_CHECK( INPUT_DIGITAL_PIN, InputId) == false);

        // React on state change.
        if( ButtonPressed != buttonCurrent[ ButtonId])
        {
          if( ButtonPressed == false)
          {
            // React on button beeing released;
            buttonCycles[ ButtonId]++;
          }

          // Remember last state.
          buttonCurrent[ ButtonId] = ButtonPressed;
        }

        ButtonId++;
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
};

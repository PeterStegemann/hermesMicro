// Copyright 2007 Peter Stegemann

#pragma once

#include "Input_Rotary.h"
#include "System_Ports.h"
#include "Types.h"
#include "Utility.h"

#define INPUT_BUTTONS               INPUT_DIGITAL_PORTS

#define INPUT_BUTTON_PINS		        ( \
                                      Utility_BitValue( INPUT_DIGITAL_A) | \
                                      Utility_BitValue( INPUT_DIGITAL_B) | \
                                      Utility_BitValue( INPUT_DIGITAL_C) | \
                                      Utility_BitValue( INPUT_DIGITAL_D) | \
                                      Utility_BitValue( INPUT_DIGITAL_E) | \
                                      Utility_BitValue( INPUT_DIGITAL_F) | \
                                      Utility_BitValue( INPUT_DIGITAL_G) | \
                                      Utility_BitValue( INPUT_DIGITAL_H) \
                                    )

#define MENU_ROTARY_PINS            ( \
                                      Utility_BitValue( MENU_ROTARY_A) | \
          					      		        Utility_BitValue( MENU_ROTARY_B) | \
                                      Utility_BitValue( MENU_ROTARY_BUTTON) \
                                    )

class Input_Service
{
	private:
		bool buttonState[ INPUT_BUTTONS];
		volatile uint8_t buttonCycles[ INPUT_BUTTONS];

  	Input_Rotary rotary;
  	volatile int8_t rotarySteps;

  	bool lastRotaryState;
	  volatile uint8_t rotaryPresses;
  	volatile uint16_t rotaryButtonTime;

    void processButtons( void)
    {
      // Go trough buttons.
      uint8_t ButtonId = 0;
      uint8_t Input = INPUT_DIGITAL_PIN;

      for( uint8_t InputId = INPUT_DIGITAL_A; InputId <= INPUT_DIGITAL_H; InputId++)
      {
        bool ButtonPressed = ( Utility_GetBit( Input, InputId) == false);

        // React on state change.
        if( ButtonPressed != buttonState[ ButtonId])
        {
          if( ButtonPressed == false)
          {
            // React on button beeing released;
            buttonCycles[ ButtonId]++;
          }

          // Remember last state.
          buttonState[ ButtonId] = ButtonPressed;
        }

        ButtonId++;
      }
    }

    void processRotary( void)
    {
      uint8_t RotaryInput = MENU_ROTARY_PIN;
      uint8_t RotaryDifference = rotary.CalculateDifference
      (
        Utility_GetBit( RotaryInput, MENU_ROTARY_A),
        Utility_GetBit( RotaryInput, MENU_ROTARY_B)
      );

      rotarySteps += RotaryDifference;

      // Rotary button
      bool RotaryButtonPressed = ( Utility_GetBit( RotaryInput, MENU_ROTARY_BUTTON) == 0);

      if( RotaryButtonPressed == true)
      {
        rotaryButtonTime++;
      }

      // React on state change.
      if( RotaryButtonPressed != lastRotaryState)
      {
        if( RotaryButtonPressed == false)
        {
          // React on button being released;
          rotaryPresses++;
        }
        else
        {
          // Start time count anew.
          rotaryButtonTime = 0;
        }

        // Remind last state.
        lastRotaryState = RotaryButtonPressed;
      }
    }

	public:
    Input_Service( void)
      : rotarySteps( 0)
      , lastRotaryState( false)
	    , rotaryPresses( 0)
      , rotaryButtonTime( 0)
    {
    }

		void Initialize( void)
    {
      for( uint8_t Count = 0; Count < INPUT_BUTTONS; Count++)
      {
        buttonState[ Count] = false;
        buttonCycles[ Count] = 0;
      }

      // Input.
      INPUT_DIGITAL_DDR &= ~INPUT_BUTTON_PINS;
      MENU_ROTARY_DDR &= ~MENU_ROTARY_PINS;

      // Pullups.
      INPUT_DIGITAL_PORT |= INPUT_BUTTON_PINS;
      MENU_ROTARY_PORT |= MENU_ROTARY_PINS;
    }

		// This is for the interrupt, not for you.
    void Process( void)
    {
      processButtons();
      processRotary();
    }

		// Get current value of the button.
    //
		// Cycles   press & release cycles since last call
		// State    current button state
		void GetButton( uint8_t ButtonId, uint8_t* Cycles = NULL, bool* State = NULL)
    {
      if( Cycles != NULL)
      {
        *Cycles = buttonCycles[ ButtonId];
        buttonCycles[ ButtonId] = 0;
      }

      if( State != NULL)
      {
        *State = buttonState[ ButtonId];
      }
    }

    // Get current value of the rotary.
    //
    // Steps          steps since last call
    // Presses        press & release cycles since last call
    // State          current button state
    // ButtonTime     last button press duration in milliseconds
    void GetRotary( int8_t* Steps = NULL, uint8_t* Presses = NULL, bool* State = NULL,
                    uint16_t* ButtonTime = NULL)
    {
      if( Steps != NULL)
      {
        *Steps = rotarySteps;
        rotarySteps = 0;

        /*
        if( rotarySteps != 0)
        {
          GLOBAL.StatusService.Tick();
        }
        */
      }

      if( Presses != NULL)
      {
        *Presses = rotaryPresses;
        rotaryPresses = 0;

        /*
        if( rotaryPresses != 0)
        {
          GLOBAL.StatusService.Beep();
        }
        */
      }

      if( State != NULL)
      {
        *State = lastRotaryState;
      }

      if( ButtonTime != NULL)
      {
        *ButtonTime = rotaryButtonTime;
      }
    }

    uint8_t GetRawInput( void)
    {
      return( INPUT_DIGITAL_PIN & INPUT_BUTTON_PINS);
    }

    uint8_t GetRawRotary( void)
    {
      return( MENU_ROTARY_PIN & MENU_ROTARY_PINS);
    }

    int8_t GetRotarySubSteps( void)
    {
      return( rotary.GetSubSteps());
    }
};

// Copyright 2007 Peter Stegemann

#pragma once

#include "Input_Rotary.h"
#include "System_Ports.h"
#include "Types.h"
#include "Utility.h"

#define INPUT_SWITCHES              INPUT_DIGITAL_PORTS

#define INPUT_SWITCH_PINS		        ( \
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
  public:
    enum SwitchState
    {
      SWITCH_STATE_Up,
      SWITCH_STATE_None,
      SWITCH_STATE_Down
    };

	private:
		volatile SwitchState switchStates[ INPUT_SWITCHES];
		volatile uint8_t switchUpToggles[ INPUT_SWITCHES];
		volatile uint8_t switchDownToggles[ INPUT_SWITCHES];

  	Input_Rotary rotary;
  	volatile int8_t rotarySteps;

  	bool lastMenuButtonState;
	  volatile uint8_t menuButtonPresses;
  	volatile uint16_t menuButtonTime;

    void processButtons( void)
    {
      // Pull down.
      INPUT_DIGITAL_PORT &= ~INPUT_SWITCH_PINS;
      uint8_t SwitchStatesDown = INPUT_DIGITAL_PIN;

      // Pull up.
      INPUT_DIGITAL_PORT |= INPUT_SWITCH_PINS;
      uint8_t SwitchStatesUp = INPUT_DIGITAL_PIN;

      // Go trough switches.
      for( uint8_t SwitchId = 0; SwitchId <= INPUT_SWITCHES; SwitchId++)
      {
        // Get current state.
        SwitchState NewSwitchState;

        if( Utility_GetBit( SwitchStatesDown, SwitchId))
        {
          NewSwitchState = SWITCH_STATE_Up;
        }
        else if( Utility_GetBit( SwitchStatesUp, SwitchId))
        {
          NewSwitchState = SWITCH_STATE_Down;
        }
        else
        {
          NewSwitchState = SWITCH_STATE_None;
        }

        // React on state change.
        if( NewSwitchState != switchStates[ SwitchId])
        {
          if( switchStates[ SwitchId] == SWITCH_STATE_Up)
          {
            switchUpToggles[ SwitchId]++;
          }
          else if( switchStates[ SwitchId] == SWITCH_STATE_Down)
          {
            switchDownToggles[ SwitchId]++;
          }

          // Remember current state.
          switchStates[ SwitchId] = NewSwitchState;
        }
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
      bool MenuButtonPressed = ( Utility_GetBit( RotaryInput, MENU_ROTARY_BUTTON) == 0);

      if( MenuButtonPressed == true)
      {
        menuButtonTime++;
      }

      // React on state change.
      if( MenuButtonPressed != lastMenuButtonState)
      {
        if( MenuButtonPressed == false)
        {
          // React on button being released;
          menuButtonPresses++;
        }
        else
        {
          // Start time count anew.
          menuButtonTime = 0;
        }

        // Remind last state.
        lastMenuButtonState = MenuButtonPressed;
      }
    }

	public:
    Input_Service( void)
      : rotarySteps( 0)
      , lastMenuButtonState( false)
	    , menuButtonPresses( 0)
      , menuButtonTime( 0)
    {
    }

		void Initialize( void)
    {
      for( uint8_t SwitchId = 0; SwitchId < INPUT_SWITCHES; SwitchId++)
      {
        switchStates[ SwitchId] = SWITCH_STATE_None;
        switchUpToggles[ SwitchId] = 0;
        switchDownToggles[ SwitchId] = 0;
      }

      // Input.
      INPUT_DIGITAL_DDR &= ~INPUT_SWITCH_PINS;
      MENU_ROTARY_DDR &= ~MENU_ROTARY_PINS;

      // No pullups.
      INPUT_DIGITAL_PORT &= ~INPUT_SWITCH_PINS;

      // Pullups.
      MENU_ROTARY_PORT |= MENU_ROTARY_PINS;
    }

		// This is for the interrupt, not for you.
    void Process( void)
    {
      processButtons();
      processRotary();
    }

    SwitchState GetSwitchState( uint8_t SwitchId)
    {
      if( SwitchId >= INPUT_SWITCHES)
      {
        return( SWITCH_STATE_None);
      }

      return( switchStates[ SwitchId]);
    }

    // Get switch up toggles since last call.
    uint8_t GetSwitchUpToggles( uint8_t SwitchId)
    {
      if( SwitchId >= INPUT_SWITCHES)
      {
        return( 0);
      }

      uint8_t SwitchToggles = switchUpToggles[ SwitchId];
      switchUpToggles[ SwitchId] = 0;

      return( SwitchToggles);
    }

    // Get switch down toggles since last call.
    uint8_t GetSwitchDownToggles( uint8_t SwitchId)
    {
      if( SwitchId >= INPUT_SWITCHES)
      {
        return( 0);
      }

      uint8_t SwitchToggles = switchDownToggles[ SwitchId];
      switchDownToggles[ SwitchId] = 0;

      return( SwitchToggles);
    }

    // Get menu steps since last call.
    int8_t GetMenuSteps( void)
    {
      int8_t Steps = rotarySteps;
      rotarySteps = 0;

      return( Steps);
    }

    // Get press & release cycles since last call.
    uint8_t GetMenuPresses( void)
    {
      uint8_t Presses = menuButtonPresses;
      menuButtonPresses = 0;

      return( Presses);
    }

    // Get current value of the menu button.
    //
    // Presses  press & release cycles since last call
    // State    current button state
    // Time     last button press duration in milliseconds
    void GetMenuButton( uint8_t* Presses = NULL, bool* State = NULL, uint16_t* Time = NULL)
    {
      if( Presses != NULL)
      {
        *Presses = GetMenuPresses();
      }

      if( State != NULL)
      {
        *State = lastMenuButtonState;
      }

      if( Time != NULL)
      {
        *Time = menuButtonTime;
      }
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

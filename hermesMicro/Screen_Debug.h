// Copyright 2024 Peter Stegemann

#pragma once

#include "Screen_Base.h"

class Screen_Debug : public Screen_Base
{
  private:
    int16_t rotaryPosition;

  public:
    Screen_Debug
    (
      Display_Service* DisplayService,
      Input_Service* InputService,
      Interrupt_Service* InterruptService,
      Store_Processor* StoreProcessor
    )
      : Screen_Base( DisplayService, InputService, InterruptService, StoreProcessor)
      , rotaryPosition( 0)
    {
    }

    virtual void draw( void)
    {
      displayService->SetMiniFont();

      uint8_t Top = 0;
      uint8_t LineHeight = 11;

      uint16_t Millis, Seconds;

      interruptService->GetTime( &Millis, &Seconds);

      displayService->PrintFormat
      (
        0, Top, "Debug %u:%02u:%01u",
        Seconds / 60, Seconds % 60, Millis / 100
      );
      displayService->Print( 0, Top += LineHeight, "Input: ");

      for( uint8_t ButtonId = 0; ButtonId < INPUT_BUTTONS; ButtonId++)
      {
        bool Current;

        inputService->GetButton( ButtonId, NULL, &Current);

        displayService->Print( 50 + ButtonId * 6, Top, Current == true ? "T" : "F");
      }

      displayService->PrintFormat
      (
        0, Top += LineHeight, "Input Raw: %x",
        inputService->GetRawInput()
      );

      int8_t Steps;
      bool State;
      uint16_t ButtonTime;

      inputService->GetRotary( &Steps, NULL, &State, &ButtonTime);

      rotaryPosition += Steps;

//      displayService->PrintFormat
//      (
//        0, Top += LineHeight, "Rotary: %d %d %s %d",
//        Steps, Presses, State == true ? "T" : "F", ButtonTime
//      );
      displayService->PrintFormat( 0, Top += LineHeight, "Rotary: %d", rotaryPosition);

      uint8_t RotaryRaw = inputService->GetRawRotary();

      displayService->PrintFormat
      (
        0, Top += LineHeight, "Rotary Raw: %x %d %s %s",
        inputService->GetRawRotary(),
        inputService->GetRotarySubSteps(),
        Utility_GetBit( RotaryRaw, MENU_ROTARY_A) ? "1" : "0",
        Utility_GetBit( RotaryRaw, MENU_ROTARY_B) ? "1" : "0"
      );
    }

    virtual bool act( void)
    {
      uint8_t Presses;

      inputService->GetRotary( NULL, &Presses, NULL, NULL);

      return( Presses == 0);
    }
};

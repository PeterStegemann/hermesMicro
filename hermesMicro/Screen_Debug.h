// Copyright 2024 Peter Stegemann

#pragma once

#include "Screen_Base.h"

class Screen_Debug : public Screen_Base
{
  private:
    int16_t rotaryPosition;
    uint8_t pressCount;

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
      , pressCount( 0)
    {
    }

    void draw( u8g2_t* Display)
    {
      displayService->SetFont( u8g2_font_6x10_tr);

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
      uint8_t Presses;
      bool State;
      uint16_t ButtonTime;

      inputService->GetRotary( &Steps, &Presses, &State, &ButtonTime);

      rotaryPosition += Steps;
      pressCount += Presses;

//      displayService->PrintFormat
//      (
//        0, Top += LineHeight, "Rotary: %d %d %s %d",
//        Steps, Presses, State == true ? "T" : "F", ButtonTime
//      );
      displayService->PrintFormat( 0, Top += LineHeight, "Rotary: %d %d", rotaryPosition, pressCount);

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

    bool act( void)
    {
      return( true);
    }
};

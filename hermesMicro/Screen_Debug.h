// Copyright 2024 Peter Stegemann

#pragma once

#include "Screen_Base.h"

class Screen_Debug : public Screen_Base
{
  private:
    int16_t rotaryPosition;

    enum Page
    {
      PAGE_One,
      PAGE_Two,

      PAGE_Count
    };

    Signal_Processor* signalProcessor;

    Page page;

  public:
    Screen_Debug
    (
      Display_Service* DisplayService,
      Input_Service* InputService,
      Interrupt_Service* InterruptService,
      Signal_Processor* SignalProcessor,
      Store_Processor* StoreProcessor
    )
      : Screen_Base( DisplayService, InputService, InterruptService, StoreProcessor)
      , rotaryPosition( 0)
      , signalProcessor( SignalProcessor)
      , page( 0)
    {
    }

    virtual void draw( void)
    {
      switch( page)
      {
        case PAGE_One : drawPageOne(); break;
        case PAGE_Two : drawPageTwo(); break;
      }
    }

    virtual void drawPageOne( void)
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
      displayService->Print( 0, Top + LineHeight, "Up: ");
      displayService->Print( 0, Top + 2 * LineHeight, "Down: ");

      for( uint8_t SwitchId = 0; SwitchId < INPUT_SWITCHES; SwitchId++)
      {
        uint8_t Left = 50 + SwitchId * 6;

        Input_Service::SwitchState Current = inputService->GetSwitchState( SwitchId);

        displayService->Print
        (
          Left, Top,
          Current == Input_Service::SWITCH_STATE_Up ? "H" :
          Current == Input_Service::SWITCH_STATE_Down ? "L" : "N"
        );

        displayService->PrintFormat
        (
          Left, Top + LineHeight, "%01x",
          inputService->GetSwitchUpToggles( SwitchId)
        );

        displayService->PrintFormat
        (
          Left, Top + 2 * LineHeight, "%01x",
          inputService->GetSwitchDownToggles( SwitchId)
        );
      }

      Top += 2 * LineHeight;

      int8_t Steps = inputService->GetMenuSteps();
      bool State;
      uint16_t ButtonTime;

      inputService->GetMenuButton( NULL, &State, &ButtonTime);

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

    virtual void drawPageTwo( void)
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

      for( uint8_t PortId = 0; PortId < 3; PortId++)
      {
        Top += LineHeight;

        int16_t CurrentLeft = signalProcessor->GetChannelValue( PortId * 2);
        int16_t CurrentRight = signalProcessor->GetChannelValue( PortId * 2 + 1);

        displayService->PrintFormat( 0, Top + LineHeight, "%6d  %6d", CurrentLeft, CurrentRight);
      }
    }

    virtual bool act( void)
    {
      page = ( PAGE_Count + inputService->GetMenuSteps() + page) % PAGE_Count;

      uint8_t Presses = inputService->GetMenuPresses();

      return( Presses == 0);
    }
};

// Copyright 2024 Peter Stegemann

#pragma once

#include "Screen_Base.h"

class Screen_Menu : public Screen_Base
{
  public:
    Screen_Menu
    (
      Display_Service* DisplayService,
      Input_Service* InputService,
      Interrupt_Service* InterruptService,
      Store_Processor* StoreProcessor
    )
      : Screen_Base( DisplayService, InputService, InterruptService, StoreProcessor)
    {
    }

    virtual void draw( void)
    {
        displayService->SetMediumFont();
        displayService->Print( 0, 0, "Zeile 1");
        displayService->Print( 0, 16, "Zeile 2");
        displayService->Print( 0, 32, "Zeile 3");
        displayService->Print( 0, 48, "Zeile 4");
    }

    bool act( void)
    {
      return( false);
    }
};

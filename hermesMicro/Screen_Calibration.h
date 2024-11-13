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
        displayService->SetSmallFont();
        displayService->Print( 0, 0, "Menu");
    }

    bool act( void)
    {
      return( false);
    }
};

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

    void draw( u8g2_t* Display)
    {
//      u8g2_ClearBuffer( Display);
        displayService->SetFont( u8g2_font_6x10_tr);

        u8g2_DrawStr( Display, 0, 0, "Menu");
//      u8g2_SendBuffer( Display);
    }

    bool act( void)
    {
      return( false);
    }
};

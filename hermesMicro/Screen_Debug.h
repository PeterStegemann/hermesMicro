// Copyright 2024 Peter Stegemann

#pragma once

#include "Screen_Base.h"

class Screen_Debug : public Screen_Base
{
  public:
    Screen_Debug( Display_Service* DisplayService, Input_Service* InputService, Store_Processor* StoreProcessor)
      : Screen_Base( DisplayService, InputService, StoreProcessor)
    {
    }

    void draw( u8g2_t* Display)
    {
      displayService->SetFont( u8g2_font_6x10_tr);

      u8g2_DrawStr( Display, 0, 0, "Debug");
      u8g2_DrawStr( Display, 0, 15, "Input: ");

      for( uint8_t ButtonId = 0; ButtonId < INPUT_BUTTONS; ButtonId++)
      {
        bool Current;

        inputService->GetButton( ButtonId, NULL, &Current);

        u8g2_DrawStr( Display, 50 + ButtonId * 6, 15, Current == true ? "T" : "F");
      }
    }

    bool act( void)
    {
      return( false);
    }
};

// Copyright 2024 Peter Stegemann

#pragma once

#include "Screen_Base.h"

class Screen_Status : public Screen_Base
{
  public:
    Screen_Status( Display_Service* DisplayService, Input_Service* InputService, Store_Processor* StoreProcessor)
      : Screen_Base( DisplayService, InputService, StoreProcessor)
    {
    }

    void draw( u8g2_t* Display)
    {
        displayService->SetFont( u8g2_font_6x10_tr);

        u8g2_DrawStr( Display, 0, 0, "Status");
    }

    bool act( void)
    {
      return( false);
    }
};

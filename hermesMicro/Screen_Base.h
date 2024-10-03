// Copyright 2024 Peter Stegemann

#pragma once

#include "Display_Service.h"
#include "Input_Service.h"

class Screen_Base
{
  private:
    virtual void draw( u8g2_t* Display);
    // Return false to exit screen.
    virtual bool act( void);

  protected:
    Display_Service* displayService;
    Input_Service* inputService;
    Store_Processor* storeProcessor;

	public:
    Screen_Base( Display_Service* DisplayService, Input_Service* InputService, Store_Processor* StoreProcessor)
      : displayService( DisplayService)
      , inputService( InputService)
      , storeProcessor( StoreProcessor)
    {
    }

    void Run( void)
    {
      u8g2_t* Display = displayService->GetDisplay();

      u8g2_FirstPage( Display);

      do
      {
        draw( Display);
      }
      while( u8g2_NextPage( Display));

      while( act() == true)
      {
        storeProcessor->Process();
      }
    }
};

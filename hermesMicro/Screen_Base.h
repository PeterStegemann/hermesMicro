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
    Interrupt_Service* interruptService;
    Store_Processor* storeProcessor;

	public:
    Screen_Base
    (
      Display_Service* DisplayService,
      Input_Service* InputService,
      Interrupt_Service* InterruptService,
      Store_Processor* StoreProcessor
    )
      : displayService( DisplayService)
      , inputService( InputService)
      , interruptService( InterruptService)
      , storeProcessor( StoreProcessor)
    {
    }

    void Run( void)
    {
      u8g2_t* Display = displayService->GetDisplay();

      do
      {
        // This feels a bit alien here, but we need to do this on a regular basis, outside of the
        // interrupts.
        storeProcessor->Process();

        u8g2_FirstPage( Display);

        do
        {
          draw( Display);
        }
        while( u8g2_NextPage( Display));
      }
      while( act() == true);
    }
};

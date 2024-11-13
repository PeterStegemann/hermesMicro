// Copyright 2024 Peter Stegemann

#pragma once

#include "Screen_Base.h"

class Screen_Status : public Screen_Base
{
  public:
    Screen_Status
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
      // Time
      displayService->SetLargeFont();

      uint16_t Seconds;

      interruptService->GetTime( NULL, &Seconds);

      displayService->PrintFormat
      (
        displayService->GetHorizontalCenter( "00:00"), 0,
        "%02u:%02u", Seconds / 60, Seconds % 60
      );

      // Battery
      displayService->HorizontalFillGauge
      (
        Display_Service::GAUGE_SIZE + 3,
        26,
        displayService->GetWidth() - 2 * Display_Service::GAUGE_SIZE - 6
      );

      // Modell name
      displayService->SetSmallFont();
      displayService->PrintCentered( 38, "Riser 100");

      // Trimmers
      uint8_t GaugeLength = 55;

      displayService->VerticalKnobGauge( 0, 0, GaugeLength);
      displayService->HorizontalKnobGauge
      (
        Display_Service::GAUGE_SIZE,
        displayService->GetHeight() - Display_Service::GAUGE_SIZE - 1,
        GaugeLength
      );
      displayService->HorizontalKnobGauge
      (
        displayService->GetWidth() - GaugeLength - Display_Service::GAUGE_SIZE,
        displayService->GetHeight() - Display_Service::GAUGE_SIZE - 1,
        GaugeLength
      );
      displayService->VerticalKnobGauge
      (
        displayService->GetWidth() - Display_Service::GAUGE_SIZE,
        0,
        GaugeLength
      );
    }

    virtual bool act( void)
    {
      uint8_t Presses;

      inputService->GetRotary( NULL, &Presses, NULL, NULL);

      return( Presses == 0);
    }
};

// Copyright 2024 Peter Stegemann

#pragma once

#include "Screen_Base.h"

#include "Signal_Processor.h"

class Screen_Status : public Screen_Base
{
  private:
    const uint8_t gaugeLength = 55;

    enum Page
    {
      PAGE_Overview,
      PAGE_ChannelsOne,
      PAGE_ChannelsTwo,
      PAGE_ChannelsThree,

      PAGE_Count
    };

    Signal_Processor* signalProcessor;

    Page page;

  public:
    Screen_Status
    (
      Display_Service* DisplayService,
      Input_Service* InputService,
      Interrupt_Service* InterruptService,
      Signal_Processor* SignalProcessor,
      Store_Processor* StoreProcessor
    )
      : Screen_Base( DisplayService, InputService, InterruptService, StoreProcessor)
      , signalProcessor( SignalProcessor)
      , page( 0)
    {
    }

    virtual void draw( void)
    {
      switch( page)
      {
        case PAGE_Overview : drawOverview(); break;

        case PAGE_ChannelsOne : drawChannels( 0); break;
        case PAGE_ChannelsTwo : drawChannels( 1); break;
        case PAGE_ChannelsThree : drawChannels( 2); break;
      }
    }

    void drawOverview( void)
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
        Display_Service::GAUGE_SIZE + 3, 26,
        displayService->GetWidth() - 2 * Display_Service::GAUGE_SIZE - 6,
        55
      );

      // Modell name
      displayService->SetSmallFont();
      displayService->PrintCentered( 38, "Riser 100");

      // Trimmers
      uint8_t HorizontalHeight = displayService->GetHeight() - Display_Service::GAUGE_SIZE - 1;

      displayService->VerticalKnobGauge( 0, 0, gaugeLength, trimValue( 0));
      displayService->HorizontalKnobGauge
      (
        Display_Service::GAUGE_SIZE, HorizontalHeight,
        gaugeLength,
        trimValue( 1)
      );
      displayService->HorizontalKnobGauge
      (
        displayService->GetWidth() - gaugeLength - Display_Service::GAUGE_SIZE, HorizontalHeight,
        gaugeLength,
        trimValue( 2)
      );
      displayService->VerticalKnobGauge
      (
        displayService->GetWidth() - Display_Service::GAUGE_SIZE, 0,
        gaugeLength,
        trimValue( 3)
      );

//      displayService->PrintFormat( 0, 0, "%0x", signalProcessor->GetTrimValue( 2));
    }

    uint8_t trimValue( uint8_t Index)
    {
      return( trimValueToGaugeLength( signalProcessor->GetTrimValue( Index)));
    }

    uint8_t trimValueToGaugeLength( int8_t TrimValue)
    {
      uint16_t Result = ( TrimValue - SIGNAL_TRIM_MINIMUM_VALUE)
                      * ( gaugeLength - 4)
                      / SIGNAL_TRIM_VALUE_RANGE;

      return( Result);
    }

    void drawChannels( uint8_t SetId)
    {
      // Time
      displayService->SetMediumFont();

      for( uint8_t LineId = 0; LineId < 3; LineId++)
      {
        uint8_t Line = LineId * 24;
        uint8_t ChannelId = LineId + SetId * 3;

        displayService->PrintFormat( 0, Line, "%1u", ChannelId + 1);

        displayService->HorizontalKnobGauge
        (
          20,
          Line + 5,
          displayService->GetWidth() - 20,
          channelValue( ChannelId)
        );
      }
    }

    uint8_t channelValue( uint8_t Id)
    {
      return( channelValueToGaugeLength( signalProcessor->GetChannelValue( Id)));
    }

    uint8_t channelValueToGaugeLength( int16_t SignalValue)
    {
      uint16_t Result = ( SignalValue - SIGNAL_MINIMUM_VALUE)
                      * ( displayService->GetWidth() - 20 - 4)
                      / SIGNAL_VALUE_RANGE;

      return( Result);
    }

    virtual bool act( void)
    {
      page = ( PAGE_Count + inputService->GetMenuSteps() + page) % PAGE_Count;

      uint8_t Presses = inputService->GetMenuPresses();

      return( Presses == 0);
    }
};

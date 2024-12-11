// Copyright 2007 Peter Stegemann

#pragma once

#include "Input_Service.h"
#include "Output_Engine.h"
#include "Setup_Service.h"
#include "Signal_Defines.h"
#include "Signal_Calibration.h"
#include "Signal_Output.h"
#include "Signal_Trim.h"
#include "System_Ports.h"
#include "Utility.h"

#define SIGNAL_PROCESSOR_CALCULATIONS_PER_SECOND		45

class Signal_Processor
{
	private:
    Input_Service* inputService;
    Output_Engine* outputEngine;
    Setup_Service* setupService;
    Status_Engine* statusEngine;

		// This signals the process routine that it is already running. It's used for overrun
    // protection.
		volatile bool inProcessing;

		// Raw and calibrated values.
		volatile int16_t rawValue[ INPUT_ANALOG_PORTS];
		volatile int16_t channelValue[ SIGNAL_PROCESSOR_CHANNELS];

		// Calibration engines.
		volatile Signal_Calibration calibration[ INPUT_ANALOG_PORTS];

		// Trim engines.
		volatile Signal_Trim trim[ SIGNAL_TRIMS];

		// Output engines.
		volatile Signal_Output output[ OUTPUT_CHANNELS];

  private:
    void processAnalogInputs( void)
    {
      // Get input values.
      for( uint8_t PortId = 0; PortId < INPUT_ANALOG_PORTS; PortId++)
      {
        // Select pin ADCx using MUX and AVCC.
        ADMUX = PortId | Utility_BitValue( REFS0);

        // Start conversion
        ADCSRA |= Utility_BitValue( ADSC);

        // Wait until conversion completed.
        while( ADCSRA & Utility_BitValue( ADSC));

        // Get converted value.
        rawValue[ PortId] = ADCW;

        // Get calibrated value.
        int16_t CalculatedValue = calibration[ PortId].Calibrate( rawValue[ PortId]);

        if( PortId < SIGNAL_PROCESSOR_CHANNELS)
        {
          channelValue[ PortId] = CalculatedValue;
        }
      }
    }

    void processOutputs( void)
    {
      // Set output values.
      for( uint8_t OutputId = 0; OutputId < OUTPUT_CHANNELS; OutputId++)
      {
        Signal_Output* Output = &( output[ OutputId]);
        uint8_t ChannelId = Output->Setup.ChannelId;

        // Get calculated value.
        int16_t CalculatedValue = Output->Calculate( GetChannelValue( ChannelId));

        outputEngine->SetChannel( OutputId, CalculatedValue);
      }
    }

	public:
    Signal_Processor
    (
      Input_Service* InputService,
      Output_Engine* OutputEngine,
      Setup_Service* SetupService,
      Status_Engine* StatusEngine
    )
      : inputService( InputService)
      , outputEngine( OutputEngine)
      , setupService( SetupService)
      , statusEngine( StatusEngine)
      , inProcessing( false)
    {
    }

		// Initialize processing.
		void Initialize( void)
    {
      for( uint8_t TrimId = 0; TrimId < SIGNAL_TRIMS; TrimId++)
      {
        trim[ TrimId].Initialize( inputService, setupService, statusEngine);
      }

      // Load calibration.
      for( uint8_t CalibrationId = 0; CalibrationId < INPUT_ANALOG_PORTS; CalibrationId++)
      {
        setupService->GetCalibration( CalibrationId, &( calibration[ CalibrationId].Setup));
      }

      // Load trim.
      for( uint8_t TrimId = 0; TrimId < SIGNAL_TRIMS; TrimId++)
      {
        setupService->GetTrim( TrimId, &( trim[ TrimId].Setup));
      }

      // Load output.
      for( uint8_t OutputId = 0; OutputId < OUTPUT_CHANNELS; OutputId++)
      {
        setupService->GetOutput( OutputId, &( output[ OutputId].Setup));
      }

      // Activate ADC with Prescaler 128 --> 14Mhz/128 = 115.2kHz
      ADCSRA = Utility_BitValue( ADEN) | \
               Utility_BitValue( ADPS2) | \
               Utility_BitValue( ADPS1) | \
               Utility_BitValue( ADPS0);
    }

		// This is for the interrupt, not for you.
		void Process( void)
    {
      if( inProcessing)
      {
        return;
      }

      inProcessing = true;

      // Process trims.
//      for( uint8_t TrimId = 0; TrimId < SIGNAL_TRIMS; TrimId++)
//      {
//        trim[ TrimId].Process( TrimId);
//      }

      // If engine is not ready, we won't calculate new data.
      if( outputEngine->ReadyForData() == false)
      {
        // Give way for the next processing loop.
        inProcessing = false;

        return;
      }

      processAnalogInputs();
      processOutputs();

      // Activate new channels.
      outputEngine->SetChannelsValid();

      // Give way for the next processing loop.
      inProcessing = false;
    }

		// Get calibration object.
    Signal_Calibration* GetCalibration( uint8_t CalibrationId)
    {
    	return( &( calibration[ CalibrationId]));
    }

		// Get trim object.
		Signal_Trim* GetTrim( uint8_t TrimId)
    {
      return( &( trim[ TrimId]));
    }

		// Get a raw input value.
		int16_t GetRawValue( uint8_t Index) const
    {
      if( Index >= INPUT_ANALOG_PORTS)
      {
        // Invalid index returns the default value.
        return( SIGNAL_NEUTRAL_VALUE);
      }

      // Read value until it stops changing for the rare case we read the value while it is
      // written.
      int16_t Value;

      do
      {
        Value = rawValue[ Index];
      }
      while( Value != rawValue[ Index]);

      return( Value);
    }

		// Get a channel output value.
		int16_t GetChannelValue( uint8_t Index) const
    {
      if( Index >= SIGNAL_PROCESSOR_CHANNELS)
      {
        // Invalid index returns the default value.
        return( SIGNAL_NEUTRAL_VALUE);
      }

      // Read value until it stops changing for the rare case we read the value while it is
      // written.
      int16_t Value;

      do
      {
        Value = channelValue[ Index];
      }
      while( Value != channelValue[ Index]);

      return( Value);
    }

    int8_t* GetTrimValue( uint8_t Index)
    {
      if( Index >= SIGNAL_TRIMS)
      {
        // Invalid index returns the default value.
        return( SIGNAL_TRIM_NEUTRAL_VALUE);
      }

      return( trim[ Index].Setup.Center);
    }
};

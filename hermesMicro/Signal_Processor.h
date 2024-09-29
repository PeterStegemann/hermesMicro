// Copyright 2007 Peter Stegemann

#pragma once

#include "Input_Processor.h"
#include "Output_Engine.h"
#include "Ports.h"
#include "Setup_Service.h"
#include "Signal_Defines.h"
#include "Signal_Calibration.h"
#include "Signal_Trim.h"
#include "Utility.h"

#define SIGNAL_PROCESSOR_CALCULATIONS_PER_SECOND		45

class Signal_Processor
{
	private:
    Input_Processor* inputProcessor;
    Output_Engine* outputEngine;
    Setup_Service* setupService;

		// This signals the process routine that it is already running. It's used for overrun
		// protection.
		volatile bool inProcessing;

		// Raw and calibrated values.
		volatile int16_t rawValue[ INPUT_ANALOG_PORTS];

		// Calibration engines.
		Signal_Calibration calibration[ INPUT_ANALOG_PORTS];

		// Calibration engines.
		Signal_Trim trim[ SIGNAL_TRIMS];

	public:
    Signal_Processor
    (
      Input_Processor* InputProcessor,
      Output_Engine* OutputEngine,
      Setup_Service* SetupService,
      Status_Engine* StatusEngine
    )
      : inputProcessor( InputProcessor)
      , outputEngine( OutputEngine)
      , setupService( SetupService)
      , inProcessing( false)
    {
      for( uint8_t TrimId = 0; TrimId < SIGNAL_TRIMS; TrimId++)
      {
        trim[ TrimId].Initialize( InputProcessor, SetupService, StatusEngine);
      }
    }

		// Initialize processing.
		void Initialize( void)
    {
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

      // Activate ADC with Prescaler 128 --> 14Mhz/128 = 115.2kHz
      ADCSRA = BIT_VALUE( ADEN) | BIT_VALUE( ADPS2) | BIT_VALUE( ADPS1) | BIT_VALUE( ADPS0);
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
      for( uint8_t TrimId = 0; TrimId < SIGNAL_TRIMS; TrimId++)
      {
        trim[ TrimId].Process( TrimId);
      }

      // If engine is not ready, we won't calculate new data.
      if( outputEngine->ReadyForData() == false)
      {
        // Give way for the next processing loop.
        inProcessing = false;

        return;
      }

      // Get input values.
      for( uint8_t Index = 0; Index < INPUT_ANALOG_PORTS; Index++)
      {
        // Select pin ADCx using MUX and AVCC.
        ADMUX = Index | BIT_VALUE( REFS0);

        // Start conversion
        ADCSRA |= BIT_VALUE( ADSC);

        // Wait until converstion completed.
        while( ADCSRA & BIT_VALUE( ADSC));

        // Get converted value.
        rawValue[ Index] = ADCW;

        // Get calibrated value.
        int16_t CalculatedValue = calibration[ Index].Calibrate( rawValue[ Index]);

        // Trim the lowest 4 channels.
        if( Index < SIGNAL_TRIMS)
        {
          CalculatedValue = trim[ Index].Trim( CalculatedValue);
        }
/*
        // Invert the lowest 3 channels if needed.
        if( Index < SIGNAL_INVERTS)
        {
          bool Invert = false;

          inputProcessor->GetSwitch( Index, &Invert);

          if( Invert == true)
          {
            CalculatedValue = -CalculatedValue;
          }
        }
*/
        outputEngine->SetChannel( Index, CalculatedValue);
      }

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
		int16_t GetRawValue( uint8_t InputId) const
    {
      if( InputId >= INPUT_ANALOG_PORTS)
      {
        // Invalid input ids return the default value.
        return( SIGNAL_NEUTRAL_VALUE);
      }
      
      int16_t Value = rawValue[ InputId];

      while( Value != rawValue[ InputId])
      {
        Value = rawValue[ InputId];
      }

      return( Value);
    }
};

// Copyright 2010 Peter Stegemann

#include "Utility.h"

#include <avr/interrupt.h>
#include <avr/wdt.h>

#include "Display_Service.h"
#include "Input_Service.h"
#include "Interrupt_Service.h"
#include "Output_Engine.h"
#include "Setup_Service.h"
#include "Screen_Debug.h"
#include "Screen_Menu.h"
#include "Screen_Status.h"
#include "Signal_Processor.h"
#include "Status_Engine.h"
#include "Store_Processor.h"
#include "System.h"

class HermesMicro
{
  private:
    Display_Service displayService;
		Input_Service inputService;
		Interrupt_Service interruptService;
		Output_Engine outputEngine;
		Setup_Service setupService;
		Signal_Processor signalProcessor;
		Status_Engine statusEngine;
		Store_Processor storeProcessor;

    void doDebug( void)
    {
        Screen_Debug ScreenDebug
        (
          &displayService,
          &inputService,
          &interruptService,
          &signalProcessor,
          &storeProcessor
        );
        ScreenDebug.Run();
    }

    void doMenu( void)
    {
        Screen_Menu ScreenMenu( &displayService, &inputService, &interruptService, &storeProcessor);
        ScreenMenu.Run();
    }

    void doStatus( void)
    {
        Screen_Status ScreenStatus
        (
          &displayService, &inputService, &interruptService, &signalProcessor, &storeProcessor
        );
        ScreenStatus.Run();
    }
/*
		void doLimit( void)
    {
      // Get input values.
      for( uint8_t Index = 0; Index < SIGNAL_LIMITS; Index++)
      {
        int16_t RawValue = signalProcessor.GetRawValue( Index);

        // Check first two channels for current position. Set limit if off center.
        int16_t CalculatedValue = signalProcessor.GetCalibration( Index)->Calibrate( RawValue);

        Signal_Trim* Trim = signalProcessor.GetTrim( Index);

        if( CalculatedValue > ( SIGNAL_MAXIMUM_VALUE * 0.65))
        {
          Trim->SetLimit( SIGNAL_TRIM_MAXIMUM_VALUE);

          limit( false);
        }
        else if( CalculatedValue < ( SIGNAL_MINIMUM_VALUE * 0.65))
        {
          Trim->SetLimit( SIGNAL_TRIM_MAXIMUM_VALUE / 2);

          limit( true);
        }
      }
    }

		void doCalibration( void)
    {
      statusEngine.SetLedState( Status_Engine::LED_BLINK_FAST);
      statusEngine.Beep();

      // Wait for user to release button.
      bool CurrentButton;

      do
      {
        inputProcessor.GetButton( INPUT_CALIBRATION_BUTTON, NULL, &CurrentButton);
      }
      while( CurrentButton == true);

      // Set center positions and prepare high and low.
      for( uint8_t CalibrationId = 0; CalibrationId < INPUT_ANALOG_PORTS; CalibrationId++)
      {
        Signal_Calibration* Calibration = signalProcessor.GetCalibration( CalibrationId);

        Calibration->Setup.High =
        Calibration->Setup.Center =
        Calibration->Setup.Low = signalProcessor.GetRawValue( CalibrationId);
      }

      statusEngine.SetLedState( Status_Engine::LED_ON);

      // Do until button gets pressed again.
      do
      {
        // Set new high and low positions.
        for( uint8_t CalibrationId = 0; CalibrationId < INPUT_ANALOG_PORTS; CalibrationId++)
        {
          Signal_Calibration* Calibration = signalProcessor.GetCalibration( CalibrationId);

          int16_t Current = signalProcessor.GetRawValue( CalibrationId);

          if( Current < Calibration->Setup.Low)
          {
            Calibration->Setup.Low = Current;
          }
          else if( Current > Calibration->Setup.High)
          {
            Calibration->Setup.High = Current;
          }
        }

        inputProcessor.GetButton( INPUT_CALIBRATION_BUTTON, NULL, &CurrentButton);
      }
      while( CurrentButton != true);	

      // Store calibration.
      for( uint8_t CalibrationId = 0; CalibrationId < INPUT_ANALOG_PORTS; CalibrationId++)
      {
        Signal_Calibration* Calibration = signalProcessor.GetCalibration( CalibrationId);

        setupService.SetCalibration( CalibrationId, &( Calibration->Setup));
      }

      // Reset trims.
      for( uint8_t TrimId = 0; TrimId < SIGNAL_TRIMS; TrimId++)
      {
        Signal_Trim* Trim = signalProcessor.GetTrim( TrimId);

        Trim->Reset();

        setupService.SetTrim( TrimId, &( Trim->Setup));
      }

      statusEngine.SetLedState( Status_Engine::LED_OFF);
    }

		void limit( bool Limit)
    {
      statusEngine.Beep();

      if( Limit == true)
      {
        statusEngine.SetLedState( Status_Engine::LED_BLINK_SUPER_FAST);
      }
      else
      {
        statusEngine.SetLedState( Status_Engine::LED_BLINK_FAST);
      }

      Utility::Pause( 250);
      statusEngine.SetLedState( Status_Engine::LED_OFF);
      Utility::Pause( 250);
    }
*/
	public:
		HermesMicro( void)
      : interruptService( &inputService, &signalProcessor, &statusEngine, &storeProcessor)
      , outputEngine( &setupService)
      , signalProcessor( &inputService, &outputEngine, &setupService, &statusEngine)
      , storeProcessor( &signalProcessor)
    {
    }

		// Go.
		void Run( void)
    {
      // Initialize display.
      displayService.Initialize();

      setupService.Reset();

      // Initialize input.
      inputService.Initialize();
      inputService.Process();

      // Check bind button.
      bool BindMode = false;
/*

      inputProcessor.GetButton( INPUT_BIND_BUTTON, NULL, &BindMode);
*/
      // Start status.
      statusEngine.Initialize();
      statusEngine.SetLedState( Status_Engine::LED_ON);

      // Enable interrupts.
      sei();

      // Run output engine.
      outputEngine.Start( BindMode);

      // Initialize processing.
      signalProcessor.Initialize();

      // Run interrupt service.
      interruptService.Start();
/*
      statusEngine.LongBeep();

      // Wait a moment to get all services going. That's especially important for input, as we want to
      // ask for a button soon.
      Utility::Pause( 10);

      // Check button turn on state.
      bool CurrentButton;

      inputProcessor.GetButton( INPUT_CALIBRATION_BUTTON, NULL, &CurrentButton);

      if( CurrentButton == true)
      {
        doCalibration();
      }

      doLimit();

      statusEngine.SetLedState( Status_Engine::LED_BLINK);
*/
      // Loop forever.
      while( true)
      {
        doStatus();
        doDebug();
//        doMenu();

        /*
        int16_t RawValue = signalProcessor.GetRawValue( 0);

        int16_t CalculatedValue = signalProcessor.GetCalibration( 0)->Calibrate( RawValue);

        if( CalculatedValue > ( SIGNAL_MAXIMUM_VALUE * 0.65))
        {
          statusEngine.SetLedState( Status_Engine::LED_BLINK_SUPER_FAST);
        }
        else if( CalculatedValue < ( SIGNAL_MINIMUM_VALUE * 0.65))
        {
          statusEngine.SetLedState( Status_Engine::LED_BLINK_FAST);
        }
        else
        {
          statusEngine.SetLedState( Status_Engine::LED_OFF);
        }
        */
      }
    }
};

static HermesMicro hermesMicro;

int main( void)
{
	// Disable watchdog.
	wdt_disable();

	hermesMicro.Run();

	return( 0);
}


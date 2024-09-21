// Copyright 2010 Peter Stegemann

#include "Utility.h"

#include <avr/interrupt.h>
#include <avr/wdt.h>

#include "Input_Processor.h"
#include "Interrupt_Service.h"
#include "Output_Engine.h"
#include "Signal_Processor.h"
#include "Status_Engine.h"
#include "Store_Processor.h"
#include "Setup_Service.h"
#include "System.h"

class hermesMicro
{
	public:
		Input_Processor InputProcessor;
		Interrupt_Service InterruptService;
		Output_Engine OutputEngine;
		Setup_Service SetupService;
		Signal_Processor SignalProcessor;
		Status_Engine StatusEngine;
		Store_Processor StoreProcessor;

	private:
		void doLimit( void)
    {
      // Get input values.
      for( uint8_t Index = 0; Index < SIGNAL_LIMITS; Index++)
      {
        int16_t RawValue = SignalProcessor.GetRawValue( Index);

        // Check first two channels for current position. Set limit if off center.
        int16_t CalculatedValue = SignalProcessor.GetCalibration( Index)->Calibrate( RawValue);

        Signal_Trim* Trim = SignalProcessor.GetTrim( Index);

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
      StatusEngine.SetLedState( Status_Engine::LEDS_BLINK_FAST);
      StatusEngine.Beep();

      // Wait for user to release button.
      bool CurrentButton;

      do
      {
        InputProcessor.GetButton( INPUT_CALIBRATION_BUTTON, NULL, &CurrentButton);
      }
      while( CurrentButton == true);

      // Set center positions and prepare high and low.
      for( uint8_t CalibrationId = 0; CalibrationId < INPUT_ANALOG_PORTS; CalibrationId++)
      {
        Signal_Calibration* Calibration = SignalProcessor.GetCalibration( CalibrationId);

        Calibration->Setup.High =
        Calibration->Setup.Center =
        Calibration->Setup.Low = SignalProcessor.GetRawValue( CalibrationId);
      }

      StatusEngine.SetLedState( Status_Engine::LEDS_ON);

      // Do until button gets pressed again.
      do
      {
        // Set new high and low positions.
        for( uint8_t CalibrationId = 0; CalibrationId < INPUT_ANALOG_PORTS; CalibrationId++)
        {
          Signal_Calibration* Calibration = SignalProcessor.GetCalibration( CalibrationId);

          int16_t Current = SignalProcessor.GetRawValue( CalibrationId);

          if( Current < Calibration->Setup.Low)
          {
            Calibration->Setup.Low = Current;
          }
          else if( Current > Calibration->Setup.High)
          {
            Calibration->Setup.High = Current;
          }
        }

        InputProcessor.GetButton( INPUT_CALIBRATION_BUTTON, NULL, &CurrentButton);
      }
      while( CurrentButton != true);	

      // Store calibration.
      for( uint8_t CalibrationId = 0; CalibrationId < INPUT_ANALOG_PORTS; CalibrationId++)
      {
        Signal_Calibration* Calibration = SignalProcessor.GetCalibration( CalibrationId);

        SetupService.SetCalibration( CalibrationId, &( Calibration->Setup));
      }

      // Reset trims.
      for( uint8_t TrimId = 0; TrimId < SIGNAL_TRIMS; TrimId++)
      {
        Signal_Trim* Trim = SignalProcessor.GetTrim( TrimId);

        Trim->Reset();

        SetupService.SetTrim( TrimId, &( Trim->Setup));
      }

      StatusEngine.SetLedState( Status_Engine::LEDS_OFF);
    }

		void limit( bool Limit)
    {
      StatusEngine.Beep();

      if( Limit == true)
      {
        StatusEngine.SetLedState( Status_Engine::LEDS_BLINK_SUPER_FAST);
      }
      else
      {
        StatusEngine.SetLedState( Status_Engine::LEDS_BLINK_FAST);
      }

      Utility::Pause( 250);
      StatusEngine.SetLedState( Status_Engine::LEDS_OFF);
      Utility::Pause( 250);
    }

	public:
		hermesMicro( void)
      : InterruptService( &InputProcessor, &SignalProcessor, &StatusEngine, &StoreProcessor)
      , OutputEngine( &SetupService)
      , SignalProcessor( &InputProcessor, &OutputEngine, &SetupService, &StatusEngine)
      , StoreProcessor( &SignalProcessor)
    {
    }

		// Go.
		void Run( void)
    {
      // Initialize input.
      InputProcessor.Initialize();
      InputProcessor.Process();

      // Check bind button.
      bool BindMode;

      InputProcessor.GetButton( INPUT_BIND_BUTTON, NULL, &BindMode);

      // Start status.
      StatusEngine.Initialize();
      StatusEngine.SetLedState( Status_Engine::LEDS_ON);

      // Enable interrupts.
      sei();

      // Run output engine.
      OutputEngine.Start( BindMode);

      // Initialize processing.
      SignalProcessor.Initialize();

      // Run interrupt service.
      InterruptService.Start();

      StatusEngine.LongBeep();

      // Wait a moment to get all services going. That's especially important for input, as we want to
      // ask for a button soon.
      Utility::Pause( 10);

      // Check button turn on state.
      bool CurrentButton;

      InputProcessor.GetButton( INPUT_CALIBRATION_BUTTON, NULL, &CurrentButton);

      if( CurrentButton == true)
      {
        doCalibration();
      }

      doLimit();

      StatusEngine.SetLedState( Status_Engine::LEDS_BLINK);

      // Loop forever.
      while( true)
      {
        /*
        int16_t RawValue = SignalProcessor.GetRawValue( 0);

        int16_t CalculatedValue = SignalProcessor.GetCalibration( 0)->Calibrate( RawValue);

        if( CalculatedValue > ( SIGNAL_MAXIMUM_VALUE * 0.65))
        {
          StatusEngine.SetLedState( Status_Engine::LEDS_BLINK_SUPER_FAST);
        }
        else if( CalculatedValue < ( SIGNAL_MINIMUM_VALUE * 0.65))
        {
          StatusEngine.SetLedState( Status_Engine::LEDS_BLINK_FAST);
        }
        else
        {
          StatusEngine.SetLedState( Status_Engine::LEDS_OFF);
        }

        Utility::Pause( 500);
        */
      }
    }
};

static hermesMicro HermesMicro;

int main( void)
{
	// Disable watchdog.
	wdt_disable();

	HermesMicro.Run();

	return( 0);
}

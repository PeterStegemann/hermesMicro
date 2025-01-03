// Copyright 2010 Peter Stegemann

#pragma once

#include "Input_Service.h"
#include "Setup_Service.h"
#include "Setup_Trim.h"
#include "Signal_Defines_Trim.h"
#include "Status_Engine.h"

class Signal_Trim
{
	private:
    Input_Service* inputService;
		Setup_Service* setupService;
		Status_Engine* statusEngine;

		// Signals whether the setup of this source was modified since the last time it was stored /
		// loaded.
		bool modified;

	public:
		Setup_Trim Setup;

	public:
		Initialize( Input_Service* InputService, Setup_Service* SetupService, Status_Engine* StatusEngine)
    {
      inputService = InputService;
      setupService = SetupService;
      statusEngine = StatusEngine;

      Reset();
    }

		void Reset( void)
    {
      Setup.Center = SIGNAL_TRIM_NEUTRAL_VALUE;

      modified = false;
    }

		void Process( uint8_t Id)
    {
      #if defined( NO_TRIM)
      Setup.Center = SIGNAL_TRIM_NEUTRAL_VALUE;
      return;
      #endif

      uint8_t UpCycles = inputService->GetSwitchUpToggles( Id);
      uint8_t DownCycles = inputService->GetSwitchDownToggles( Id);

      int8_t Steps = UpCycles - DownCycles;

      if( Steps == 0)
      {
        return;
      }

      int8_t NewValue = Setup.Center;
      NewValue += Steps;

      if( NewValue < SIGNAL_TRIM_MINIMUM_VALUE)
      {
        NewValue = SIGNAL_TRIM_MINIMUM_VALUE;

        statusEngine->Beep();
      }
      else if( NewValue > SIGNAL_TRIM_MAXIMUM_VALUE)
      {
        NewValue = SIGNAL_TRIM_MAXIMUM_VALUE;

        statusEngine->Beep();
      }
      else
      {
        Setup.Center = NewValue;
        modified = true;

        if( NewValue == SIGNAL_TRIM_NEUTRAL_VALUE)
        {
          statusEngine->Beep();
        }
        else
        {
          statusEngine->Tick();
        }
      }
    }

		void StoreIfModified( uint8_t Id)
    {
      if( modified == true)
      {
        modified = false;

        setupService->SetTrim( Id, &Setup);
      }
    }

		int16_t Trim( int16_t Value)
    {
      // Move value into positive realm.
      uint16_t UnsignedValue = ( int32_t) Value - ( int32_t) SIGNAL_MINIMUM_VALUE;

      // Calculate width of one map column.
      uint16_t Width = (( uint16_t) SIGNAL_VALUE_RANGE) / 2;

      // Calculate in which column this value is.
      int32_t Lower, Higher;

      // Lower half.
      if( Value < 0)
      {
        Lower = SIGNAL_MINIMUM_VALUE;
        Higher = Setup.Center;
      }
      // Upper half.
      else if( Value < SIGNAL_MAXIMUM_VALUE)
      {
        Lower = Setup.Center;
        Higher = SIGNAL_MAXIMUM_VALUE;
      }
      // Top point.
      else
      {
        int32_t Result = SIGNAL_MAXIMUM_VALUE;
        Result *= SIGNAL_VALUE_RANGE;
        Result /= SIGNAL_TRIM_VALUE_RANGE;

        return( Result);
      }

      // Blow up to signal level.
      Lower *= SIGNAL_VALUE_RANGE;
      Lower /= SIGNAL_TRIM_VALUE_RANGE;
      Higher *= SIGNAL_VALUE_RANGE;
      Higher /= SIGNAL_TRIM_VALUE_RANGE;

      // Now calculate where in the column it is.
      int32_t Where = UnsignedValue % Width;

      int32_t Rise = Higher - Lower;
      int32_t Full = Where * Rise;
      Full /= Width;

      int16_t Result = Lower + Full;

      return( Result);
    }
};

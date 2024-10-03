// Copyright 2010 Peter Stegemann

#pragma once

#include "Signal.h"
#include "Signal_Calibration.h"
#include "Signal_Processor.h"
#include "Signal_Trim.h"

class Store_Processor
{
	private:
    Signal_Processor* signalProcessor;

		// This signals the process routine that it is already running. It's used for overrun
		// protection.
		volatile bool inProcessing;

	public:
		// Initialize processing.
	  Store_Processor( Signal_Processor* SignalProcessor)
      : signalProcessor( SignalProcessor)
      , inProcessing( false)
    {
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
        signalProcessor->GetTrim( TrimId)->StoreIfModified( TrimId);
      }

      // Give way for the next processing loop.
      inProcessing = false;
    }
};

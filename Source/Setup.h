// Copyright 2008 Peter Stegemann

#pragma once

#include "Input_Defines.h"
#include "Ports.h"
#include "Setup.h"
#include "Setup_Calibration.h"
#include "Setup_Trim.h"
#include "Signal_Defines.h"
#include "Signal_Trim_Defines.h"

#include <avr/eeprom.h>

struct Setup
{
	uint8_t InvertedPPM;

	Setup_Calibration Calibration[ INPUT_ANALOG_PORTS];

	Setup_Trim Trim[ SIGNAL_TRIMS];
};

Setup Eeprom EEMEM =
{
	0,

	{
		{ 1023, 511, 0},
		{ 1023, 511, 0},
		{ 1023, 511, 0},
		{ 1023, 511, 0},
		{ 1023, 511, 0},
		{ 1023, 511, 0}
	},

	{
		{ SIGNAL_TRIM_MINIMUM_VALUE, SIGNAL_TRIM_NEUTRAL_VALUE, SIGNAL_TRIM_MAXIMUM_VALUE, false},
		{ SIGNAL_TRIM_MINIMUM_VALUE, SIGNAL_TRIM_NEUTRAL_VALUE, SIGNAL_TRIM_MAXIMUM_VALUE, false},
		{ SIGNAL_TRIM_MINIMUM_VALUE, SIGNAL_TRIM_NEUTRAL_VALUE, SIGNAL_TRIM_MAXIMUM_VALUE, false},
		{ SIGNAL_TRIM_MINIMUM_VALUE, SIGNAL_TRIM_NEUTRAL_VALUE, SIGNAL_TRIM_MAXIMUM_VALUE, false}
	}
};

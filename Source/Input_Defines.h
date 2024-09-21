// Copyright 2024 Peter Stegemann

#pragma once

#define INPUT_BUTTONS               8

#define INPUT_CALIBRATION_BUTTON    0
#define INPUT_BIND_BUTTON           1

#define INPUT_SWITCHES              3

#define INPUT_BUTTON_INPUT_PINS		  ( BIT_VALUE( INPUT_TRIM_INPUT_0) | BIT_VALUE( INPUT_TRIM_INPUT_1) | BIT_VALUE( INPUT_TRIM_INPUT_2) | BIT_VALUE( INPUT_TRIM_INPUT_3))
#define INPUT_BUTTON_OUTPUT_PINS	  ( BIT_VALUE( INPUT_TRIM_OUTPUT_0) | BIT_VALUE( INPUT_TRIM_OUTPUT_1))

#define INPUT_SWITCH_PINS           ( BIT_VALUE( INPUT_INVERT_ANALOG_A) | BIT_VALUE( INPUT_INVERT_ANALOG_B) | BIT_VALUE( INPUT_INVERT_ANALOG_C))

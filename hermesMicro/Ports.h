// Copyright 2010 Peter Stegemann

#pragma once

#include "System.h"

#include <avr/io.h>

// Input misc
#define INPUT_TIMERA                TCCR0A
#define INPUT_TIMERB                TCCR0B

#define INPUT_ANALOG_PORTS          6

#define INPUT_ANALOG_A_D_PORT       PORTC
#define INPUT_ANALOG_A_D_DDR        DDRC
#define INPUT_ANALOG_A_D_PIN        PINC

#define INPUT_TRIM_PORTS            6

#define INPUT_TRIM_PORT             PORTD
#define INPUT_TRIM_DDR              DDRD
#define INPUT_TRIM_PIN              PIND

#define INPUT_TRIM_OUTPUT_0         PD2
#define INPUT_TRIM_OUTPUT_1         PD3
#define INPUT_TRIM_INPUT_0          PD4
#define INPUT_TRIM_INPUT_1          PD5
#define INPUT_TRIM_INPUT_2          PD6
#define INPUT_TRIM_INPUT_3          PD7

// Display
#define DISPLAY_PORT                PORTB
#define DISPLAY_SCK                 PB5
#define DISPLAY_MOSI                PB3

// Serial communication
#define SERIAL_PORT                 PORTD
#define SERIAL_DDR                  DDRD
#define SERIAL_PIN                  PIND

#define SERIAL_RX                   PD0
#define SERIAL_TX                   PD1

// PPM generation
#define PPM_TIMERA                  TCCR1A
#define PPM_PORT                    PORTB
#define PPM_DDR                     DDRB
#define PPM_PIN                     PINB

#define PPM_SIGNAL                  PB1

// Status
#if defined( HAS_STATUS_LED)
  #define STATUS_PORT               PORTB
  #define STATUS_DDR                DDRB

  #define STATUS_LED                PB5
#endif

// Beep
#define BUZZER_PORT                 PORTB
#define BUZZER_DDR                  DDRB

#define BUZZER_NOISE                PB0

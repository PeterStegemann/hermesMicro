// Copyright 2010 Peter Stegemann

#pragma once

#include "System.h"

#include <avr/io.h>

// Input misc
#define INPUT_ANALOG_PORTS          5

#define INPUT_ANALOG_PORT           PORTC
#define INPUT_ANALOG_DDR            DDRC
#define INPUT_ANALOG_PIN            PINC

#define INPUT_ANALOG_A              PC0
#define INPUT_ANALOG_B              PC1
#define INPUT_ANALOG_C              PC2
#define INPUT_ANALOG_D              PC3
#define INPUT_ANALOG_E              PC6

#define INPUT_DIGITAL_PORTS         8

#define INPUT_DIGITAL_PORT          PORTD
#define INPUT_DIGITAL_DDR           DDRD
#define INPUT_DIGITAL_PIN           PIND

#define INPUT_DIGITAL_A             PD0
#define INPUT_DIGITAL_B             PD1
#define INPUT_DIGITAL_C             PD2
#define INPUT_DIGITAL_D             PD3
#define INPUT_DIGITAL_E             PD4
#define INPUT_DIGITAL_F             PD5
#define INPUT_DIGITAL_G             PD6
#define INPUT_DIGITAL_H             PD7

// Menu
#define MENU_ROTARY_PORT            PORTB
#define MENU_ROTARY_DDR             DDRB
#define MENU_ROTARY_PIN             PINB
#define MENU_ROTARY_A               PB2
#define MENU_ROTARY_B               PB3
#define MENU_ROTARY_BUTTON          PB4

// Display
#define DISPLAY_PORT                PORTC
#define DISPLAY_SDA                 PC4
#define DISPLAY_SCL                 PC5

// Serial communication
#define SERIAL_PORT                 PORTD
#define SERIAL_DDR                  DDRD
#define SERIAL_PIN                  PIND

#define SERIAL_RX                   PD0
#define SERIAL_TX                   PD1

// PPM generation
#define PPM_PORT                    PORTB
#define PPM_DDR                     DDRB

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

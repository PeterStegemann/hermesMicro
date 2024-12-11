// Copyright 2024 Peter Stegemann

#pragma once

#include "System.h"

#if defined( OUTPUT_SPEKTRUM)
  #include "Output_Spektrum_Defines.h"

  #define OUTPUT_CHANNELS   SPEKTRUM_CHANNELS
#endif

#if defined( OUTPUT_PPM)
  #include "Output_Ppm_Defines.h"

  #define OUTPUT_CHANNELS   PPM_CHANNELS
#endif

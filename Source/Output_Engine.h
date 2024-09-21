// Copyright 2024 Peter Stegemann

#pragma once

#include "System.h"

#if defined( OUTPUT_SPEKTRUM)
  #include "Output_Spektrum_Engine.h"

  #define Output_Engine     Output_Spektrum_Engine
#endif

#if defined( OUTPUT_PPM)
  #include "Output_Ppm_Engine.h"

  #define Output_Engine     Output_Ppm_Engine
#endif

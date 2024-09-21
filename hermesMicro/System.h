// Copyright 2015 Peter Stegemann

#pragma once

#define NO_TRIM
//#define OUTPUT_SPEKTRUM
#define OUTPUT_PPM

#if( defined OUTPUT_SPEKTRUM && defined OUTPUT_PPM)
  #error "Only one of OUTPUT_SPEKTRUM or OUTPUT_PPM may be used"
#endif

#if( !defined OUTPUT_SPEKTRUM && !defined OUTPUT_PPM)
  #error "At least one of OUTPUT_SPEKTRUM or OUTPUT_PPM has to be used"
#endif
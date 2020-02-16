#pragma once

#define BUILD_TYPE_MOCK 9
#define BUILD_TYPE_ROOT 3
#define BUILD_TYPE_NODE 2

#ifndef BUILD_TYPE
  #error Must define BUILD_TYPE...
#endif


#if BUILD_TYPE == BUILD_TYPE_MOCK
  #ifdef ESP32
    #define DATA_PIN 23
    #define CLOCK_PIN -1
    #define TOUCH_PIN 27
  #elif ESP8266
  #endif
#elif BUILD_TYPE == BUILD_TYPE_NODE
  #ifdef ESP32
    #define DATA_PIN 23
    #define CLOCK_PIN -1
    #define TOUCH_PIN 27
  #elif ESP8266
  #endif
#elif BUILD_TYPE == BUILD_TYPE_ROOT
  #ifdef ESP32
    #define DATA_PIN 23
    #define CLOCK_PIN -1
    #define TOUCH_PIN 27
  #endif
#endif

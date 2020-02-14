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
    #define RX_PIN RX
    #define TX_PIN TX

    #define CLOCK_PIN D4
    #define DATA_PIN D3
    #define RST_PIN -1

    #define sharpLEDPin D0
    #define sharpVoPin A0
    #define SHARP_GND_PIN D5
    #define SHARP_VCC_PIN D6
  #endif
#elif BUILD_TYPE == BUILD_TYPE_NODE
  #ifdef ESP8266
    #define CLOCK_PIN -1
    #define DATA_PIN D3
    #define RST_PIN -1
  #endif
#elif BUILD_TYPE == BUILD_TYPE_ROOT
  #ifdef ESP32
    #define DATA_PIN 23
    #define CLOCK_PIN -1
    #define TOUCH_PIN 27
  #endif
#endif


/*******************************************************************
*                                                                  *
*     @file         main.c                                         *
*                                                                  *
*     @authors      Joel Howell                                    *
*                   Matthew Panizza                                *
*                                                                  *
*     @date         Created Monday, June 29th, 2020                *
*                                                                  *
*     @brief        A simple tetris game written in C.             *
*                   main.c  controls main game.                    *
*                                                                  *
*     @copyright    Copyright (c) 2020 Joel Howell. All rights     *
*                   reserved. Licensed under the MIT License.      *
*                   See LICENSE for details.                       *
*                                                                  *
*******************************************************************/	

#include "neopixel.h"

/////////////////////////////////////
// NEOPIXEL MATRIX CHARACTERISTICS //
/////////////////////////////////////

#define PIXEL_COUNT 256         //Number of pixels in matrix

#define PIXEL_PIN A2            //Define Hardware pin used for data

#define PIXEL_TYPE WS2812B       //Define LED Type

Adafruit_NeoPixel strip = Adafruit_NeoPixel(PIXEL_COUNT, PIXEL_PIN, PIXEL_TYPE);    //Initialize neopixel function


////////////////////////////////////
//// HARDWARE CONFIGURATION ////////
////////////////////////////////////

SYSTEM_MODE(AUTOMATIC);         //Tells device to use WiFi by default

#define lbtn D0                //Defines button configuration for navigation buttons

#define rbtn D1

#define dnbtn D2

#define ebtn D3

#define spkpn D4                //Define Speaker pin (optional)

#define brtsns A4               //Define Photoresistor pin (use 10k pulldown resistor)



void setup() {
  // Put initialization like pinMode and begin functions here.

}

// loop() runs over and over again, as quickly as it can execute.
void loop() {
  // The core of your code will likely live here.

}
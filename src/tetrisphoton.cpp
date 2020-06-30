/******************************************************/
//       THIS IS A GENERATED FILE - DO NOT EDIT       //
/******************************************************/

#include "Particle.h"
#line 1 "c:/Users/mligh/OneDrive/Particle/TetrisPhoton/tetrisphoton/src/tetrisphoton.ino"

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

void setup();
int getRVal(int colorCode);
int getGVal(int colorCode);
int getBVal(int colorCode);
void printBoard(int usrBoard[]);
void loop();
#line 26 "c:/Users/mligh/OneDrive/Particle/TetrisPhoton/tetrisphoton/src/tetrisphoton.ino"
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


////////////////////////////////////
////// GLOBAL VARIABLE LIST ////////
////////////////////////////////////

int i;                          //Multipurpose counter variables (i and j)
int j;
int tetris[PIXEL_COUNT];        //Array to hold Tetris board

void setup() {
    strip.begin();
    strip.show();
    for(i = 0; i < PIXEL_COUNT; i++){
        tetris[i] = 0;
    }
    delay(100);
    tetris[24] = 1255000000;
    tetris[25] = 1000255000;
    tetris[26] = 1000000255;
}
int getRVal(int colorCode){                                                                                         //Filters out Red value from array element
    return ((colorCode/1000000)%1000);
}
int getGVal(int colorCode){                                                                                         //Filters out Green value from array element
    return ((colorCode/1000)%1000);
}
int getBVal(int colorCode){                                                                                         //Filters out Blue value from array element
    return (colorCode%1000);
}
void printBoard(int usrBoard[]){
    for(i = 0; i < PIXEL_COUNT; i++){
        if(((i/16)%2) == 0){                                                                                        //Prints standard row
            strip.setPixelColor(i,getRVal(tetris[i]),getGVal(tetris[i]), getBVal(tetris[i]));
        }
        else{                                                                                                       //Reversed row for S configuration (mirror function)
            if((i%16) < 8){
                strip.setPixelColor(i+15-((i%16)*2),getRVal(tetris[i]),getGVal(tetris[i]), getBVal(tetris[i]));     //If on the right half of line, add to mirror to other side
            }
            else{
                strip.setPixelColor(i+15-(2*(i%16)),getRVal(tetris[i]),getGVal(tetris[i]), getBVal(tetris[i]));     //If on the left half of line, subtract to mirror to other side
            }
        }
    }
    strip.show();
}
// loop() runs over and over again, as quickly as it can execute.
void loop() {
  // The core of your code will likely live here.
    printBoard(tetris);
    delay(5000);
}

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
//#include "tetris.h"

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

SYSTEM_MODE(MANUAL);         //Tells device to use WiFi by default

#define lbtn D0                //Defines button configuration for navigation buttons

#define rbtn D1

#define dnbtn D2

#define ebtn D3

#define spkpn D4                //Define Speaker pin (optional)

#define brtsns A4               //Define Photoresistor pin (use 10k pulldown resistor)



void loadPiece(int pid, int orientation, int* off);

bool movePiece(int* currSpot, int direction, int pid, int orientation, int pieceColor, int* userBoard);

void rotatePiece(int currSpot, int pid, int * currOrientation, int newOrientation, int pieceColor, int* userBoard);

bool isSpotLegal(int newSpot, int pid, int orientation, int* userBoard);

void checkForRows(int numRows, int numColumns, int* userBoard);

void shiftRowsDown(int emptyRow, int numRows, int numColumns, int* userBoard);

int getPieceColor(int pid, int level, int brtlvl);




////////////////////////////////////
////// GLOBAL VARIABLE LIST ////////
////////////////////////////////////

int i;                          //Multipurpose counter variables (i and j)
int j;
int pieceSpot;
int pieceRotation;
int currentPiece;
int tetris[PIXEL_COUNT];        //Array to hold Tetris board

void setup() {
    pinMode(lbtn, INPUT_PULLDOWN);
    pinMode(rbtn, INPUT_PULLDOWN);
    pinMode(dnbtn, INPUT_PULLDOWN);
    pinMode(ebtn, INPUT_PULLDOWN);
    pinMode(D4, INPUT_PULLDOWN);
    strip.begin();
    strip.show();
    for(i = 0; i < PIXEL_COUNT; i++){
        tetris[i] = 0;
    }
    pieceSpot = 8;
    pieceRotation = 0;

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
void gameDone(){
    delay(20000);
}
void waitForButton(int timeDelay, int * currSpot, int pid, int * orientation, int pieceColor, int* userBoard){
    int myTemp;
    for(myTemp = 0; myTemp < (timeDelay/10); myTemp++){
        if(digitalRead(lbtn) == HIGH){
            movePiece(currSpot,1,pid, *orientation,pieceColor, userBoard);
            printBoard(tetris);
            while(digitalRead(lbtn) == HIGH){
                delay(5);
            }
        }
        else if(digitalRead(rbtn) == HIGH){
            movePiece(currSpot, 2, pid, *orientation, pieceColor, userBoard);
            printBoard(tetris);
            while(digitalRead(rbtn) == HIGH){
                delay(5);
            }
        }
        else if(digitalRead(dnbtn) == HIGH){
            myTemp += 10;
            delay(5);
        }
        else if(digitalRead(ebtn) == HIGH){
            if(*orientation < 3){
                rotatePiece(*currSpot, pid, orientation, (*orientation)+1, pieceColor, userBoard);
            }
            else{
                rotatePiece(*currSpot, pid, orientation, 0, pieceColor, userBoard);
            }
            printBoard(tetris);
            while(digitalRead(ebtn) == HIGH){
                delay(5);
            }
        }
        else if(digitalRead(D4) == HIGH){
            System.dfu();
        }
        delay(10);
    }
    return;
}
void loop() {
    currentPiece = random(7);
    pieceSpot = 8;
    pieceRotation = 0;
    if(!placePiece(pieceSpot,currentPiece,getPieceColor(currentPiece, 0, 20),tetris)){
        gameDone();
    }
    printBoard(tetris);
    waitForButton(2000, &pieceSpot, currentPiece, &pieceRotation, getPieceColor(currentPiece, 0, 20), tetris);
    while(!movePiece(&pieceSpot, 0, currentPiece, pieceRotation, getPieceColor(currentPiece, 0, 20), tetris)){
        waitForButton(2000, &pieceSpot, currentPiece, &pieceRotation, getPieceColor(currentPiece, 0, 20), tetris);
        printBoard(tetris);
    }
    delay(1000);
}


void loadPiece(int pid, int orientation, int* off){
    switch (pid){
        case 0:         //Square, top left corner is origin
            off[0] = 0;
            off[1] = 1;
            off[2] = 16;
            off[3] = 17;
            break;
        case 1:                                         //Vertical piece, middle block is origin
            if(orientation == 0 || orientation == 2){       //Horizontal orientation
                off[0] = -1;                                    //
                off[1] = 0;                                     //
                off[2] = 1;                                     //
                off[3] = 2;                                     //
            }
            else{                                           //Vertical Orientation
                off[0] = -16;                                   //
                off[1] = 0;                                     //
                off[2] = 16;                                    //
                off[3] = 32;                                    //
            }
            break;
        case 2:                                         //L-Shape
            if(orientation == 0){                           //Horizontal orientation, middle of l is origin
                off[0] = -1;                                    //
                off[1] = 0;                                     //
                off[2] = 1;                                     //
                off[3] = 15;                                    //
            }
            else if(orientation == 1){
                off[0] = -17;
                off[1] = -16;
                off[2] = 0;
                off[3] = 16;
            }
            else if(orientation == 2){
                off[0] = -1;
                off[1] = 0;
                off[2] = 1;
                off[3] = -15;
            }
            else{                                       //Vertical Orientation
                off[0] = -16;
                off[1] = 0;
                off[2] = 16;
                off[3] = 17;
            }
            break;
        case 3:                                         //Reverse-L
            if(orientation == 0){                       //Horizontal orientation, middle of l is origin
                off[0] = -1;
                off[1] = 0;
                off[2] = 1;
                off[3] = 17;
            }
            else if(orientation == 1){
                off[0] = -16;
                off[1] = 0;
                off[2] = 16;
                off[3] = 15;
            }
            else if(orientation == 2){
                off[0] = -17;
                off[1] = -1;
                off[2] = 0;
                off[3] = 1;
            }
            else{                                       //Vertical Orientation
                off[0] = -16;
                off[1] = -15;
                off[2] = 0;
                off[3] = 16;
            }
            break;
        case 4:                                         //T-Shape
            if(orientation == 0){                       
                off[0] = -1;
                off[1] = 0;
                off[2] = 1;
                off[3] = 16;
            }
            else if(orientation == 1){
                off[0] = -16;
                off[1] = 0;
                off[2] = -1;
                off[3] = 16;
            }
            else if(orientation == 2){
                off[0] = -16;
                off[1] = 0;
                off[2] = 1;
                off[3] = 16;
            }
            else{                                       //Vertical Orientation
                off[0] = -16;
                off[1] = -1;
                off[2] = 0;
                off[3] = 1;
            }
            break;
        case 5:                                         //S-zig-zag
            if(orientation == 0){                       //Horizontal orientation, middle of l is origin
                off[0] = 0;
                off[1] = 1;
                off[2] = 16;
                off[3] = 15;
            }
            else if(orientation == 1){
                off[0] = -17;
                off[1] = -1;
                off[2] = 0;
                off[3] = 16;
            }
            else if(orientation == 2){
                off[0] = -16;
                off[1] = -15;
                off[2] = 0;
                off[3] = -1;
            }
            else{                                       //Vertical Orientation
                off[0] = -16;
                off[1] = 0;
                off[2] = 1;
                off[3] = 17;
            }
            break;
        case 6:                                         //Z-zig-zag
            if(orientation == 0){                       //Horizontal orientation, middle of l is origin
                off[0] = 0;
                off[1] = -1;
                off[2] = 16;
                off[3] = 17;
            }
            else if(orientation == 1){
                off[0] = -16;
                off[1] = -1;
                off[2] = 0;
                off[3] = 15;
            }
            else if(orientation == 2){
                off[0] = -17;
                off[1] = -16;
                off[2] = 0;
                off[3] = 1;
            }
            else{                                       //Vertical Orientation
                off[0] = -15;
                off[1] = 0;
                off[2] = 1;
                off[3] = 16;
            }
            break;
    }
}

bool placePiece(int newSpot, int pid, int pieceColor, int* userBoard){
    int tempPiece[4];
    loadPiece(pid, 0, tempPiece);
    if(isSpotLegal(newSpot, pid, 0, userBoard)){
        for(int i = 0; i < 4; i++){                                             //Fill given color into the new location of the piece
            userBoard[newSpot+tempPiece[i]] = pieceColor;
        }
        return true;
    }
    else{
        for(int i = 0; i < 4; i++){                                             //Fill given color into the new location of the piece
            userBoard[newSpot+tempPiece[i]] = pieceColor;
        }
        return false;
    }
    
}

int getPieceColor(int pid, int level, int brtlvl){
    switch (pid)
    {
        case 0:
            return (1000000000+brtlvl);
            break;
        case 1:
            return (1000000000+brtlvl+(1000*brtlvl));
            break;
        case 2:
            return (1000000000+brtlvl+(1000*(brtlvl/2)));
            break;
        case 3:
            return (1000000000+(brtlvl/2)+(1000*(brtlvl/5)));
            break;    
        case 4:
            return (1000000000+(brtlvl/2)+(1000*brtlvl));
            break; 
        case 5:
            return (1000000000+(brtlvl/5)+(1000*brtlvl));
            break; 
        case 6:
            return (1000000000+(brtlvl/3)+(1000*(brtlvl/3))+(1000000*(brtlvl/3)));
            break; 
    }
}

bool movePiece(int* currSpot, int direction, int pid, int orientation, int pieceColor, int* userBoard){            //Returns true if at bottom
    int tempPiece[4];
    loadPiece(pid, orientation, tempPiece);
    for(int i = 0; i < 4; i++){                                             //Erase current color value
        userBoard[*currSpot+tempPiece[i]] = 0;
    }
    switch (direction){
        case 0:                                                                 //Piece being moved down (automatic or not)
            if(!isSpotLegal(*currSpot+16, pid, orientation, userBoard)){              //Checks if piece is touching another piece along its bottom edge
                for(int i = 0; i < 4; i++){                                             //Erase current color value
                    userBoard[*currSpot+tempPiece[i]] = pieceColor;
                }
                checkForRows(16,16,userBoard);
                return true;                                                            //If it is touching another piece, tell caller that it can drop next piece
            }
            else{                                                                   //Otherwise, move piece down in array
                for(int i = 0; i < 4; i++){                                             //Erase current color value
                    userBoard[*currSpot+tempPiece[i]] = 0;
                }
                for(int i = 0; i < 4; i++){                                             //Fill given color into the new location of the piece
                    userBoard[*currSpot+tempPiece[i]+16] = pieceColor;
                }
                *currSpot = *currSpot+ 16;
                break;                                                          //Caller is told that piece can keep moving
            }
        case 1:                                                                 //Piece being moved left
            if(isSpotLegal(*currSpot+1, pid, orientation, userBoard) && canShiftPiece(*currSpot, 1, pid, orientation, pieceColor, userBoard)){
                for(int i = 0; i < 4; i++){                                             //Erase current color value
                    userBoard[*currSpot+tempPiece[i]] = 0;
                }
                for(int i = 0; i < 4; i++){                                             //Fill given color into the new location of the piece
                    userBoard[*currSpot+tempPiece[i]+1] = pieceColor;
                }
                *currSpot = *currSpot+1;
                return false;
            }
            else{
                for(int i = 0; i < 4; i++){                                             //Erase current color value
                    userBoard[*currSpot+tempPiece[i]] = pieceColor;
                }
                break;
            }
        case 2:                                                                 //Piece being moved right
            if(isSpotLegal(*currSpot-1, pid, orientation, userBoard) && canShiftPiece(*currSpot, 2, pid, orientation, pieceColor, userBoard)){
                for(int i = 0; i < 4; i++){                                             //Erase current color value
                    userBoard[*currSpot+tempPiece[i]] = 0;
                }
                for(int i = 0; i < 4; i++){                                             //Fill given color into the new location of the piece
                    userBoard[*currSpot+tempPiece[i]-1] = pieceColor;
                }
                *currSpot = *currSpot-1;
                return false;
            }
            else{
                for(int i = 0; i < 4; i++){                                             //Erase current color value
                    userBoard[*currSpot+tempPiece[i]] = pieceColor;
                }
                break;
            }
    }
    return false;
}

void rotatePiece(int currSpot, int pid, int* currOrientation, int newOrientation, int pieceColor, int* userBoard){
    int tempPiece[4];
    loadPiece(pid, *currOrientation, tempPiece);
    for(int i = 0; i < 4; i++){                                             //Erase current color value
        userBoard[currSpot+tempPiece[i]] = 0;
    }
    loadPiece(pid, newOrientation, tempPiece);
    if(isSpotLegal(currSpot, pid, newOrientation, userBoard)){
        for(int i = 0; i < 4; i++){                                             //Fill given color into the new rotation of the piece
            userBoard[currSpot+tempPiece[i]] = pieceColor;
        }
        *currOrientation = newOrientation;
        return;
    }
    else{
        loadPiece(pid, *currOrientation, tempPiece);
        for(int i = 0; i < 4; i++){                                             //Erase current color value
            userBoard[currSpot+tempPiece[i]] = pieceColor;
        }
        return;
    }
}

bool canShiftPiece(int currSpot, int direction, int pid, int orientation, int pieceColor, int* userBoard){
    return true;
}

bool isSpotLegal(int newSpot, int pid, int orientation, int* userBoard){      //Function to test if a new configuration is a legal change in the board, returns true if legal
    int tempPiece[4];                                                               //Temporary array to hold piece configuration offsets
    loadPiece(pid, orientation, tempPiece);                                        //Populate array with offsets
    int tempMax = tempPiece[0];
    int tempMin = tempPiece[0];
    for(i = 0; i < 4; i++){
        if(tempPiece[i] > tempMax){
            tempMax = tempPiece[i];
        }
        if(tempPiece[i] < tempMin){
            tempMin = tempPiece[i];
        }
    }
    if(newSpot + tempMax > 255 || newSpot + tempMin < 0){
        return false;
    }
    for(int i = 0; i < 4; i++){                                                     //Loop through each offset
        if(userBoard[newSpot + tempPiece[i]] > 0){                                      //Check if location is already occupied
            return false;                                                                   //If any parts are occupied, then new location is not legal
        }
    }
    return true;
}

void checkForRows(int numRows, int numColumns, int* userBoard){               //Function to scan for any complete rows that need to be erased per game rules
    int i;                                                                      //i and j are multipurpose loop variables
    int j;  
    for(i = 0; i < numRows; i++){                                               //Loop for each row of board
        bool isEqual = true;                                                            //Set bool variable to check each row, gets set false if there is an empty space
        for(j = 0; j < numRows; j++){
            if(userBoard[j+i*16] == 0){                                                          //If any spots are empty, then row is not complete
                isEqual = false;
            }
        }
        if(isEqual){                                                                    //If there is a complete row, erase it and call the function to shift rows down
            for(j = 0; j < numRows; j++){
                userBoard[j+i*numColumns] = 0;
            }
            shiftRowsDown(i, numRows, numColumns, userBoard);
        }
    }
}

void shiftRowsDown(int emptyRow, int numRows, int numColumns, int* userBoard){        //Fucntion to shift all rows above emptyRow down 1
    int i;
    for(i = emptyRow*numColumns+numColumns-1; i >= numColumns; i--){            
        userBoard[i] = userBoard[i-16];
    }
}
/**
  ******************************************************************************
  * @file    morseDecode.c
  * @author  Giuseppe Callipo - IK8YFW - ik8yfw@libero.it
  * @version V2.0.0
  * @date    25-09-2018
  * @brief   STM32 porting of WB7FHC's arduino based cw decoder routine
  *
  *
  * NOTE: This file is part of RadioDSP project.
  *       See main.c file for additional project informations.
  * Platform: CortexM4
  ******************************************************************************/

#include "stm32f4xx_conf.h"
#include "morseDecode.h"
#include "io_display.h"
/*
 * The following code is a modified version of WB7FHC's code so:
 *
 **********************************************************************
 *
 *
 *   WB7FHC's Simple Morse Code Decoder
 *   (c) 2015, Budd Churchward - WB7FHC
 *   This is an Open Source Project
 *   http://opensource.org/licenses/MIT
 *
 *   Search YouTube for 'WB7FHC' to see several videos of this project
 *   as it was developed.
 *
 *   MIT license, all text above must be included in any redistribution
 ***********************************************************************
 *
*/

uint8_t  ditOrDah = 1;                       // 0=false - 1=true We have either a full dit or a full dah
int      dit = 3;                              // We start by defining a dit as 10 milliseconds

// The following values will auto adjust to the sender's speed
int   averageDah = 15;                      // A dah should be 3 times as long as a dit
int   averageWordGap = 100;           // will auto adjust - initialize as avaragegaps
long  fullWait = 150;                       // The time between letters
long  waitWait = 150;                       // The time between dits and dahs
long  newWord = 0;                           // The time between words

uint8_t  characterDone = 1;                  // 0=false - 1=true A full character has been sent

int   downTime = 0;                          // How long the tone was on in milliseconds
int   upTime = 0;                            // How long the tone was off in milliseconds
int   myBounce = 2;                          // Used as a short delay between key up and down

long  startDownTime = 0;                     // Timer when tone first comes on
long  startUpTime = 0;                       // Timer when tone first goes off

long  lastDahTime = 0;                       // Length of last dah in milliseconds
long  lastDitTime = 0;                       // Length oflast dit in milliseconds
long  averageDahTime = 0;                    // Sloppy Average of length of dahs

uint8_t  justDid = 1;                        // 1= true 0=false Makes sure we only print one space during long gaps

int   myNum = 0;                             // Will turn dits and dahs into a binary number stored here

/////////////////////////////////////////////////////////////////////////////////
// Now here is the 'Secret Sauce'
// The Morse Code is embedded into the binary version of the numbers from 2 - 63
// The place a letter appears here matches myNum that we parsed out of the code
// #'s are miscopied characters
char mySet[] ="##TEMNAIOGKDWRUS##QZYCXBJP#L#FVH09#8###7#####/-61#######2###3#45";



char recoveredChar = ' ';                   // We store the actual character decoded here
int  peakLevel;
char morseBuffer[40];                       // Buffer to hold received morse characters
int bufferIn, bufferOut = 0;                // read/write pointers to the buffer for circular opperation

//------------------------------------------------------------------
void printPunctuation() {
  // Punctuation marks are made up of more dits and dahs than
  // letters and numbers. Rather than extend the character array
  // out to reach these higher numbers we will simply check for
  // them here. This funtion only gets called when myNum is greater than 63

  switch (myNum) {
    case 71:
      recoveredChar = ':';
      break;
    case 76:
      recoveredChar = ',';
      break;
    case 84:
      recoveredChar = '!';
      break;
    case 94:
      recoveredChar = '-';
      break;
    case 97:
      recoveredChar = 39;    // Apostrophe
      break;
    case 101:
      recoveredChar = '@';
      break;
    case 106:
      recoveredChar = '.';
      break;
    case 115:
      recoveredChar = '?';
      break;
    case 246:
      recoveredChar = '$';
      break;
    case 122:
      recoveredChar = 's';
      recoveredChar = 'k';
      break;
    default:
      recoveredChar = '#';                        // Unknown - Should never get here
      break;
  }
}

//------------------------------------------------------------------
void printCharacter() {
  justDid = 0;                                  // OK to print a space again after this

  // Punctuation marks will make a BIG myNum
  if (myNum > 63) {                                 // The value we parsed is bigger than our character array
    printPunctuation();                             // It is probably a punctuation mark so go figure it out.
    return;
  }

  // This print put a char ...
  // speechBuffer_push(mySet[myNum]);
  showMorseChar(mySet[myNum]);
}


//------------------------------------------------------------------
void printSpace() {
   if (justDid) return;                             // only one space, no matter how long the gap
   justDid = 1;
}


//---------------------------------------------------------------------------------
void shiftBits() {
  // we know we've got a dit or a dah, let's find out which
  // then we will shift the bits in myNum and then add 1 or not add 1

  if (downTime < dit / 3) return;                       // ignore my keybounce

  myNum = myNum << 1;                                   // shift bits left
  ditOrDah = 1;                                      // we will know which one in two lines


  // If it is a dit we add 1. If it is a dah we do nothing!
  if (downTime < dit) {
     myNum++;                                           // add one because it is a dit
     } else {                                           // The next three lines handle the automatic speed adjustment:
        averageDah = (downTime+averageDah) / 2;         // running average of dahs
        dit = averageDah / 3;                           // normal dit would be this
        dit = dit * 2;                                  // double it to get the threshold between dits and dahs
     }
}


//------------------------------------------------------------------------------------
 void morseKeyDown() {                              // Tone detected

   //digitalWrite(MORSE_LED, HIGH);// tone detected

   if (startUpTime>0){                              // We only need to do once, when the key first goes down
     startUpTime=0;                                 // clear the 'Key Up' timer
   }
   // If we haven't already started our timer, do it now
   if (startDownTime == 0){
       startDownTime = millis();                    // get time
   }
   characterDone=0;                             // we're still building a character
   ditOrDah=0;                                  // the key is still down we're not done with the tone
  // delay(myBounce);                                 // Take a short breath here
   if (myNum == 0) {                                // myNum will equal zero at the beginning of a character
        myNum = 1;                                  // This is our start bit  - it only does this once per letter
   }
 }


 //---------------------------------------------------------------------------------
  void morseKeyUp() {                               // No tone
    //digitalWrite(MORSE_LED, LOW); // no tone is there

    if (startUpTime == 0){startUpTime = millis();}  // If we haven't already started our timer, do it now

    // Find out how long we've gone with no tone. If it is twice as long as a dah print a space
    upTime = millis() - startUpTime;
    //if (upTime<1)return;
    if (upTime > (averageDah*2)) {
      printSpace();
    }
    if (startDownTime > 0){                        // Only do this once after the key goes up
      downTime = millis() - startDownTime;         // how long was the tone on?
      startDownTime=0;                             // clear the 'Key Down' timer
    }
    if (!ditOrDah) {                               // We don't know if it was a dit or a dah yet
      shiftBits();                                 // let's go find out! And do our Magic with the bits
    }
    if (!characterDone) {                          // If we are still building a character ...
      if (upTime > dit) {                          // Are we done yet?
        // BINGO! we're done with this one
        printCharacter();                          // Go figure out what character it was and print it
        characterDone=1;                        // We got him, we're done here
        myNum=0;                                   // This sets us up for getting the next start bit
      }
      downTime=0;                                  // Reset keyDown counter
    };
}


//---------------------------------------------------------------------------------
/*- This method initialize or reset the cw decoder */
void initializeDecoder(){

ditOrDah = 1;                       // 0=false - 1=true We have either a full dit or a full dah
dit = 3;                              // We start by defining a dit as 10 milliseconds

// The following values will auto adjust to the sender's speed
averageDah = 15;                      // A dah should be 3 times as long as a dit
averageWordGap = 100;           // will auto adjust - initialize as avaragegaps
fullWait = 150;                       // The time between letters
waitWait = 150;                       // The time between dits and dahs
newWord = 0;                           // The time between words

characterDone = 1;                  // 0=false - 1=true A full character has been sent

downTime = 0;                          // How long the tone was on in milliseconds
upTime = 0;                            // How long the tone was off in milliseconds
myBounce = 2;                          // Used as a short delay between key up and down

startDownTime = 0;                     // Timer when tone first comes on
startUpTime = 0;                       // Timer when tone first goes off

lastDahTime = 0;                       // Length of last dah in milliseconds
lastDitTime = 0;                       // Length oflast dit in milliseconds
averageDahTime = 0;                    // Sloppy Average of length of dahs

justDid = 1;                        // 1= true 0=false Makes sure we only print one space during long gaps

myNum = 0;                             // Will turn dits and dahs into a binary number stored here
}

/**************************************END OF FILE****/


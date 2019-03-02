

//#define ENABLE_MEMO_KEYER

#define ENABLE_QSOTEXT_GENERATOR
// uncomment to simulate mistakes in QSOTEXT generator
#define SIMULATE_MISTAKES

// display buffer for bottom display line, one extra element for a \0.
// NOTE: this is the place to consume as much SRAM as available,
// to allow for a long scrolling.
#define TEXT_BUFFER_LENGTH (LCD_COLUMNS*15 + 1)

/* If I understand it correctly, the USB port on an Arduino Nano is
   connected to a FTDI (or similar) USB to serial chip, instead of
   directly to the ATMega328 (or ATMega168). You can't make the USB
   anything else other than a serial port.  You could add a USB port
   using 2 digital I/O pins and control it through the V-USB library,
   but at this point it's probably a lot easier and less expensive
   buying an Arduino Pro or Arduino Pro Micro or something else. */
// NOTE that the keyboard code is untested.
//#define ENABLE_USB_KEYBOARD_OUTPUT

////////////////////////////////////////////////////////////////////////

#define metaVERSION "6.0"

/*
  3.2 was '$',
  4.0 was '@',
  4.1 was '+',
  4.2 and 4.3 had '?',
  4.4 and 4.5 have '*',
  6.0 has '.'
 */
const byte MorserSignature = '.';

// uncomment to enable serial debbugging communication
//#define DEBUG


#ifdef ENABLE_MEMO_KEYER
//The macro E2END is the last EEPROM address.
#define MAX_MEMO_TEXT_LENGTH (E2END-sizeof(byte)-sizeof(CWs)-2)
#endif

#ifdef ENABLE_USB_KEYBOARD_OUTPUT
#include <Keyboard.h>
#endif

// enable a generator which is helpful for debugging sigs: it will
// just output all sigs in order.
//#define ENABLE_TEST_GENERATOR


/*************************************************************************************
 * Copyright 2017, Willi Kraml (OE1WKL)
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy,
 * modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 ************************************************************************************/

/***********  I M P O R T A N T ! !  W I C H T I G ! !*****************************
 *  There are 2 (two) versions of
 *  hardware, currently, the only difference affecting the code is the
 *  rotary encoder older versions: uses the KY-040 encoder
 *  (recognazible as it is on a separate PCB newer versions: uses a
 *  more "standard" encoder You NEED to #define which encoder is being
 *  used - if you fail to do so, the Arduino IDE (compiler) will throw
 *  an error ar you!  For this purpose, eliminate the '//' (starts a
 *  comment line) in front of one of the following lines! (at the type
 *  that is being used)
 *
 *  Es gibt derzeit 2 (zwei) unterschiedliche Hardware Versionen, die
 *  sich vor allem im verwendeten Rotary Encoder unterscheiden die
 *  ältere Version: benutzt einen KY-040 Encoder - man erkennt ihn
 *  leicht daran, dass er auf einer separaten kleinen Platine sitzt
 *  die neuere Version: benutzt einen Encoder, der eher "Standard" ist
 *  Du MUSST mit #define festlegen, welcher Encoder benutzt wird,
 *  sonst gibt es einen Fehler zur Laufzeit!  Dazu musst du die '//'
 *  (Beginn einer Kommentarzeile) bei einer der folgenden Zeilen
 *  entfernen! (Bei dem Typ, der verwendet wird)
 */

/*  V 5.0 by DM4SG, Stefan Großhauser, dm4sg@darc.de */

/*  V 5.1 by OE9BKJ, klaus.battlogg@gmx.net Added menu 5 item to enable/disable LCD
       backlight.  Note: you have to set the LED/Backlight jumper on
       the LCD Board- see
       http://henrysbench.capnfatz.com/henrys-bench/arduino-displays/ywrobot-lcm1602-iic-v1-lcd-arduino-tutorial/
*/

/*  V 6.0 by DM4SG, Stefan Großhauser, dm4sg@darc.de */

//#define KY_040_ENCODER
#define STANDARD_ENCODER

/* the next line is a factor determining the sensitivity of the paddles */
/* lower value: higher sensitivity; range should be within 0.90 and 0.99 */

#define SENS_FACTOR 0.96

// we define auto-character spacing in the next line...
// ACS_LENGTH normally should be 3, if less strict then 2

#define ACS_LENGTH 3

/*  code by others used in this sketch:

 *  NewliquidCrystal for LCD/I2C/TWI display
 *  see https://bitbucket.org/fmalpartida/new-liquidcrystal/wiki/Home
 *  and http://arduino-info.wikispaces.com/LCD-Blue-I2C
 *  written by F. Malpartida (https://bitbucket.org/fmalpartida)

 *  Touch sensor interface by Martin Pittermann,
    see https://github.com/martin2250/ADCTouch and http://playground.arduino.cc/Code/ADCTouch

 *  ClickButton library -> https://code.google.com/p/clickbutton/ by Ragnar Aronsen

 *  Rotary Encoder code based on arduinoalemans code, with some changes
 *  see https://forum.arduino.cc/index.php?topic=242356.0

 *  For persistent storage (EEPROM) I am using the extended EEPROM library by Thijs Elenbaas
 *  http://thijs.elenbaas.net/2012/07/extended-eeprom-library-for-arduino/
 *  For volume control of NF output: Volume3 by Connor Nishijima,
 *  see https://github.com/connornishijima/arduino-volume3 and
 *  https://hackaday.io/project/11957-10-bit-component-less-volume-control-for-arduino

 *  Routines for morse decoder - to a certain degree based on code by Hjalmar Skovholm Hansen OZ1JHM - see http://skovholm.com/cwdecoder
 */

// include the library code:
#include <Wire.h>           // TWI interface
#include <LCD.h>
#include <LiquidCrystal_I2C.h>
#include "ClickButton.h"    // button control library
#include <EEPROMex.h>       // extended EEPROM library
//#define VOLUME2
#ifdef VOLUME2
#include <volume2.h>        // volume library
#error "this library is broken and does not allow to use an arbitrary pin"
#else
#include <Volume3.h>        // volume library
#endif

#include <ADCTouch.h>       // touch sensor library


#ifdef VOLUME2
Volume vol;
#endif
#define NOTE_A4_FREQ 440
#define NOTE_CSHARP5_FREQ 554
#define NOTE_E5_FREQ 659
#define NOTE_FSHARP4_FREQ 397
#define NOTE_CSHARP4_FREQ 227
#define NOTE_A3_FREQ 220


// set the LCD address for a 16 chars 2 line display.  the LCD I2C
// address is a placeholder - the real address is determined now in
// setup() !!! can be 0x27 or 0x3f
LiquidCrystal_I2C lcd(0x5f);
#define LCD_COLUMNS 16
#define LCD_ROWS 2

/// globald constants and variables
///
/// Arduino pins used:

const byte leftPin =  A2;          // connect left paddle to pin A2
const byte rightPin = A0;         // and right one to Pin A0
const byte sidetonePin = 9;      // to generate an ugly keyer side tone.
//const byte sidetonePin = 10;      // pin 10 if pin 9 has been destroyed by the speaker
const byte modeButtonPin = 4;    // input pin for mode button
// this keys the transmitter /through a 2N2222 NPN transistor - at the
// same time lights up the LED on the Arduino
const byte keyerPin = 13;
const byte straightPin = 12;     // input for straight key - for Morse decoder
const byte audioInPin = A6;      // audio in for Morse decoder

// set up the button
// initialize mode button
ClickButton modeButton(modeButtonPin, LOW, CLICKBTN_PULLUP);

const byte PinCLK=2; // Used for generating byteerrupts using CLK signal
const byte PinDT=3; // Used for reading DT signal

//this variable stores our current value of encoder position.since
// this is going to be updated by an interrupt service routine, it
// must be declared volatile, to prevent the compiler from optimizing
// it away.
volatile char encoderDelta = 0;

uint16_t signCounter;
uint16_t signCounter_inclBlanks;
uint16_t charCounter;
#define N_SIGNS_THEN_SAVE_CONFIG 30

int refLeft, refRight;       //reference values to remove offset
boolean leftKey, rightKey;
int value0, value1;


//// DISPLAY LAYOUT of top line - 16 characters wide - keyer mode
////            |0...+....9....+.|
////            |99wpm A+ .- 550∎|
//// DISPLAY LAYOUT of top line - 16 characters wide - trainer mode
////            |0...+....9....+.|
////            |99w/1 äa1.<>550∎|
////  new: Farnsworth factor as /1 instead of pm
///   instead of mode & polarity: symbols for target characters (max 6 long)
const byte lcdFarnsworth = 2;
const byte lcdSymbols = 5;
const byte lcdSpeed = 0;
const byte lcdMode = 6;
const byte lcdPolarity = 9;
const byte lcdPitch = 12;
const byte lcdVolume = 15;

char formatIntegerBuffer[3];

// define modes for state machine of the various modi the encoder can be in

enum __attribute__ ((__packed__)) encoderMode {scrollMode=0, speedSettingMode, curtisSettingMode,
    polaritySettingMode, pitchSettingMode, volumeSettingMode, numEncoderModes};

// we start with adjusting the speed
//encoderMode encoderState = speedSettingMode;
// we start with scrolling
encoderMode encoderState = scrollMode;

//Current position of the viewport. This can be used to review
//characters which have already been scrolled out of the display.
int16_t scrollPosition = 0;

/// the states the morserino can be in - top level menu
enum __attribute__ ((__packed__)) morserinoMode {morseKeyer=0,
    morseTrainer, morseDecoder, morseCopyGame,
    morseQuickEcho,
#ifdef ENABLE_MEMO_KEYER
    morseSetAutoKeyerText,
#endif
    morseBacklight,
    numMorserinoModes};
morserinoMode morseState = morseKeyer;


// scroll length is 1 less, because of the \0 marker
#define SCROLL_LENGTH (TEXT_BUFFER_LENGTH - 1 - LCD_COLUMNS)
// this will be used as queue and implemented as a circular buffer
char textBuffer[TEXT_BUFFER_LENGTH];
// topmost element of the queue, initialise with -1 to push the first
// character at position 0
int16_t textBufferIndex = -1;


// Curtis Mode A */
// Curtis Mode B */
// improved Curtis B mode */
// Ultimatic mode */
enum __attribute__ ((__packed__)) keyer_mode {KEYER_MODE_IAMBICA=0,
    KEYER_MODE_IAMBICB,
    KEYER_MODE_IAMBICBplus,
    KEYER_MODE_ULTIMATIC,
    numKeyerModes};
const char keyer_modes[][3] = {"A ","B ","B+","Ul"};

enum __attribute__ ((__packed__)) sec_mode {SEC_MODE_GENERATE=0, SEC_MODE_INPUT, SEC_MODE_PROCESS};
sec_mode secondaryMode;
uint16_t copyGame_keyedChars;
uint16_t copyGame_generatedChars;
uint16_t copyGame_keyedChars_without_blanks;
uint16_t correctCount;
boolean showMarks = true;
boolean quickEcho_wordDone;
boolean quickEcho_lastWordCorrect;

byte resultSnippetLength;
// the length of the game:
//#define COPY_GAME_N_SIGS (LCD_COLUMNS)
#define COPY_GAME_N_SIGS 15

//#define CORRECT_SYMBOL '.'
// a dot in the center of the matrix
//#define CORRECT_SYMBOL ((char)B10100101)
// a small circle
#define CORRECT_SYMBOL ((char)B11011111)
//#define MISTAKE_SYMBOL 'x'
// a small cross
#define MISTAKE_SYMBOL ((char)B11101011)

#define ARROW_RIGHT_SYMBOL ((char)126)
#define ARROW_LEFT_SYMBOL ((char)127)

// japanese sign resembling a smiley
#define SMILEY_SYMBOL1 ((char)B10101111)
#define SMILEY_SYMBOL2 ((char)B11000010)

#define COUNTDOWN_SECONDS 5
#if (COPY_GAME_N_SIGS * 4 * 2 + 2 + 9 > TEXT_BUFFER_LENGTH)
// this we may have at most 4 characters per sign, and we must store
// two lines of them in memory. To this add two \0 and the trailing
// snippet with the percentage
#error "TEXT_BUFFER_LENGTH macro must be at least 4*2 times"\
"as long as COPY_GAME_N_SIGS"
#endif



///// GROUPOF5
// 0 = A, 1 = 9, 2 = A9, 3 = A9?, 4 = ?<>, 5 = A9?<>,
// 6 = äA, 7= äA9, 8 = äA9.<>, 9 = calls, 10=q-groups & abbreviations
//
// Grouping:   a    |  9   |  .   |  <>              |  a9  | .<>              |   äa    | äa9   | a9.   | a9.<>            | äa9.<>
//            4-29  | 30-39| 40-48| 48-(BLANK_IDX-1) | 4-39 | 40-(BLANK_IDX-1) | 0-29    | 0-39  | 4-48  | 4-(BLANK_IDX-1)  | 0-(BLANK_IDX-1)

// the length of the pool array + 1 is used to indicate a blank.

// a macro to make the compiler translate from characters to pool index
#define A2IDX(c) (c < 58 ? c-18 : c-93 )


// the funny spelling of German ä: reason is that these are "local"
// characters within the LCD display, not Unicode or whatever, so I
// need to use hex escapes in order to clearly indicate where the hex
// escape ends, I need to break the string into substrings....
const char group00[] PROGMEM = " a    ";
const char group01[] PROGMEM = "  9   ";
const char group02[] PROGMEM = " a9   ";
const char group03[] PROGMEM = " a9?  ";
const char group04[] PROGMEM = "   ?<>";
const char group05[] PROGMEM = " a9?<>";
const char group06[] PROGMEM = "\xE1" "a    ";
const char group07[] PROGMEM = "\xE1" "a9   ";
const char group08[] PROGMEM = "\xE1" "a9?<>";
const char group09[] PROGMEM = "CALLs ";
const char group10[] PROGMEM = "ABBRV ";
const char group11[] PROGMEM = "CHOICE";
#define GENERATOR_MODE_KOCH_OFFSET 12
#ifdef ENABLE_QSOTEXT_GENERATOR
const char group12[] PROGMEM = "QSO   ";
#undef GENERATOR_MODE_KOCH_OFFSET
#define GENERATOR_MODE_KOCH_OFFSET 13
#endif
#ifdef ENABLE_TEST_GENERATOR
const char group13[] PROGMEM = "TEST  ";
#ifdef ENABLE_TEST_GENERATOR
#undef GENERATOR_MODE_KOCH_OFFSET
#define GENERATOR_MODE_KOCH_OFFSET 14
#else
#undef GENERATOR_MODE_KOCH_OFFSET
#define GENERATOR_MODE_KOCH_OFFSET 13
#endif
#endif
#define NUMBER_OF_TRAINING_GROUPS (GENERATOR_MODE_KOCH_OFFSET + GENERATOR_MODE_N_KOCH_LEVELS)

const char * const groups[GENERATOR_MODE_KOCH_OFFSET] PROGMEM = {
  group00,
  group01,
  group02,
  group03,
  group04,
  group05,
  group06,
  group07,
  group08,
  group09,
  group10,
  group11,
#ifdef ENABLE_QSOTEXT_GENERATOR
  group12,
#endif
#ifdef ENABLE_TEST_GENERATOR
  group13,
#endif
};

// structure to contain persistent CW settings, to be stored in EEPROM
struct CWs {
  uint8_t wpm;                      // keyer speed in wörtern pro minute
  uint16_t sidetoneFreq;        // side tone frequency
  uint16_t sidetoneVolume;           // side tone volume
  boolean didah;                // paddle polarity
  keyer_mode keyermode;               // Iambic keyer mode: see the #defines above
  uint8_t farnsworthMode;           // trainer: extend pauses by 1, 2, 3 to 9
  uint8_t generatorMode; // trainer: what symbol (groups) are we going to send?
  int tLeft;                    // threshold for left paddle
  int tRight;                   // threshold for right paddle
  // true if we use an external (mechanical) paddle instead of the touch sensors
  boolean useExtPaddle;
  boolean ACS;                  // true if we use Automatic Character Spacing
  byte i2cAddress;              // result of i2c scanner
  boolean backlightOn;          // backlight on|off setting
};

struct CWs CWsettings = {
  15, 650, 5, true, KEYER_MODE_IAMBICA, 4, 2, 110, 110, false, false, 0x3F, true
};

// we use this to see if anything in CW settings has changed, so we
// need to write settings into EEPROM
boolean settingsDirty = false;

int realVolume[] = {0, 32, 64, 160, 270, 400, 650, 800, 1023};


//  keyerControl bit definitions

#define DIT_L    0x01// Dit latch
#define DAH_L    0x02// Dah latch
#define DIT_LAST 0x04// Dit was last processed element

//  Global Keyer Variables
//

// this holds the latches for the paddles and the DIT_LAST latch, see
// above
unsigned char keyerControl = 0;


boolean DIT_FIRST = false; // first latched was dit?
unsigned int ditLength;// dit length in milliseconds - 100 = 60bpm = 12 wpm
unsigned int dahLength;        // dahs are 3 dits long

//  Iambic Keyer State Machine
enum __attribute__ ((__packed__)) KSTYPE {IDLE_STATE=0, DIT, DAH, KEY_START, KEYED, INTER_ELEMENT};
KSTYPE keyerState;


///////////////////////////////////////////////////////////////////////////////

// for each character:
// byte morse encoding as binary value, and byte length.
const byte pool[][2] PROGMEM = {
#define AE_IDX 0
  {B01010000, 4},  // ä    0
#define OE_IDX 1
  {B11100000, 4},  // ö
#define UE_IDX 2
  {B00110000, 4},  // ü
#define CH_IDX 3
  {B11110000, 4},  // ch   3
#define A_IDX 4
  {B01000000, 2},  // a    4     ASCII +93
  {B10000000, 4},  // b
  {B10100000, 4},  // c
  {B10000000, 3},  // d
  {B00000000, 1},  // e
  {B00100000, 4},  // f
  {B11000000, 3},  // g
  {B00000000, 4},  // h
  {B00000000, 2},  // i
  {B01110000, 4},  // j
  {B10100000, 3},  // k
  {B01000000, 4},  // l
#define M_IDX 16
  {B11000000, 2},  // m  16
  {B10000000, 2},  // n
  {B11100000, 3},  // o
#define P_IDX 19
  {B01100000, 4},  // p  19
  {B11010000, 4},  // q
  {B01000000, 3},  // r
  {B00000000, 3},  // s
  {B10000000, 1},  // t
  {B00100000, 3},  // u
  {B00010000, 4},  // v
  {B01100000, 3},  // w
  {B10010000, 4},  // x
  {B10110000, 4},  // y
#define Z_IDX 29
  {B11000000, 4},  // z  29
  {B11111000, 5},  // 0  30    ASCII +18
  {B01111000, 5},  // 1
  {B00111000, 5},  // 2
  {B00011000, 5},  // 3
  {B00001000, 5},  // 4
  {B00000000, 5},  // 5
  {B10000000, 5},  // 6
  {B11000000, 5},  // 7
  {B11100000, 5},  // 8
  {B11110000, 5},  // 9  39
  // Interpunkt   . , : - / = ? @ +
  //             010101 110011 111000 100001 10010 10001 001100 011010 01010
#define POINT_IDX 40
  {B01010100, 6},  // .  40    ASCII +6
#define COMMA_IDX 41
  {B11001100, 6},  // ,  41    ASCII +3
#define COLON_IDX 42
  {B11100000, 6},  // :  42    ASCII +16
#define MINUS_IDX 43
  {B10000100, 6},  // -  43    ASCII +2
#define SLASH_IDX 44
  {B10010000, 5},  // /  44    ASCII +3
#define BT_IDX 45
  {B10001000, 5},  // =  45    (at the same time <bt> ) ASCII +16
#define QUESTION_IDX 46
  {B00110000, 6},  // ?  46    ASCII +17
#define AT_IDX 47
  {B01101000, 6},  // @  47    ASCII +17
#define AR_IDX 48
  {B01010000, 5},  // +  48    (at the same time <ar> !) ASCII -5
  //#define SEMICOLON_IDX 48
  // Betriebsabkuerzungen, prosigns
  //{B10101000, 6},   // ';' semicolon
  //#define PARAGRAPH_IDX 49
  //{B01001000, 6},   // '!' exclamation mark
#define AS_IDX 49
  {B01000000, 5},  // <as> 49
#define KA_IDX 50
  {B10101000, 5},  // <ka>
#define KN_IDX 51
  {B10110000, 5},  // <kn> (at the same time '(' )
#define SK_IDX 52
  {B00010100, 6},  // <sk> 52
#define HH_IDX 53
  {B00000000, 8},  // <hh>
#define SN_IDX 54
#define VE_IDX SN_IDX
  {B00010000, 5},  // <sn> == <ve> 54
#define KK_IDX 55
  {B10110100, 6},   // <kk> 55 (at the same time ')' )
#define BK_IDX 56
  {B10001010, 7},   // <bk> 56
// what about the following ones?
  //{B01010000, 6},   // "paragraph"
};
#define NUMBER_OF_SIGNS_IN_POOL 57
#define BLANK_IDX NUMBER_OF_SIGNS_IN_POOL
#define END_OF_WORD_IDX (BLANK_IDX+1)
#define NOT_IN_POOL_IDX (BLANK_IDX+2)

// bounds of the different types of characters, as used e.g. for the group5 generator.
const byte bounds[][2] = {
#define BOUNDS_LETTER 0
  {A_IDX, Z_IDX},
#define BOUNDS_NUMBER 1
  {30, 39},
#define BOUNDS_ALPHANUMERIC 2
  {A_IDX, 39},
  {A_IDX, AR_IDX},
  {POINT_IDX, BLANK_IDX - 1}, {A_IDX, BLANK_IDX - 1}, {0, Z_IDX}, {0, 39}, {0, BLANK_IDX - 1}};

struct linklist {
  const byte dit;
  const byte dah;
  // an index with respect to the pool
  const byte sigidx;
};

#define CWTREE_UNKNOWN_SIG 69
#define CWTREE_BLANK_SIG 70
//a square, centered dot - used for all unidentifiable characters:
#define UNKNOWN_SIG_SYMB 165
const struct linklist CWtree[]  = {
#define CWTREE_ROOT 0
  {1,2,0},      // 0
  { 3,4,A2IDX('e')},     // 1
  {5,6,A2IDX('t')},      // 2
  //
  {7,8,A2IDX('i')},      // 3
  {9,10,A2IDX('a')},      // 4
  {11,12,A2IDX('n')},     // 5
  {13,14,A2IDX('m')},     // 6
  //
  {15,16,A2IDX('s')},     // 7
  {17,18,A2IDX('u')},     // 8
  {19,20,A2IDX('r')},     // 9
  {21,22,A2IDX('w')},     //10
  {23,24,A2IDX('d')},       //11
  {25,26,A2IDX('k')},      //12
  {27,28,A2IDX('g')},      //13
  {29,30,A2IDX('o')},     //14
  //---------------------------------------------
  {31,32,A2IDX('h')},     // 15
  {33,34,A2IDX('v')},      // 16
  {CWTREE_UNKNOWN_SIG, CWTREE_UNKNOWN_SIG,A2IDX('f')},      // 17
  {35, 36,UE_IDX},      // 18 german ue
  {37,38,A2IDX('l')},      // 19
  {39, CWTREE_UNKNOWN_SIG,AE_IDX},      // 20 german ae
  {CWTREE_UNKNOWN_SIG, 40,A2IDX('p')},      // 21
  {CWTREE_UNKNOWN_SIG, 41,A2IDX('j')},      // 22
  {42,43,A2IDX('b')},      // 23
  {44, CWTREE_UNKNOWN_SIG,A2IDX('x')},      // 24
  {CWTREE_UNKNOWN_SIG, 45,A2IDX('c')},      // 25
  {46, CWTREE_UNKNOWN_SIG,A2IDX('y')},      // 26
  {47,48,A2IDX('z')},      // 27
  {CWTREE_UNKNOWN_SIG, CWTREE_UNKNOWN_SIG,A2IDX('q')},      // 28
  {49, CWTREE_UNKNOWN_SIG,1},      // 29 german oe
#define CH_SYMB 1
  {50,51,3},      // 30 !!! german 'ch'
  //---------------------------------------------
  {64, CWTREE_UNKNOWN_SIG,A2IDX('5')},      // 31
  {CWTREE_UNKNOWN_SIG, CWTREE_UNKNOWN_SIG,A2IDX('4')},      // 32
  //{'*', CWTREE_UNKNOWN_SIG, 52},      // 33
#define SN_SYMB 2
  {CWTREE_UNKNOWN_SIG, 52,VE_IDX},      // 33  <ve>
  {CWTREE_UNKNOWN_SIG,CWTREE_UNKNOWN_SIG,A2IDX('3')},     // 34
  {53,CWTREE_UNKNOWN_SIG,NOT_IN_POOL_IDX},      // 35
  {CWTREE_UNKNOWN_SIG, CWTREE_UNKNOWN_SIG,A2IDX('2')},      // 36
#define AS_SYMB 3
  {CWTREE_UNKNOWN_SIG,CWTREE_UNKNOWN_SIG,AS_IDX},     // 37 !! <as>
  {54, CWTREE_UNKNOWN_SIG,NOT_IN_POOL_IDX},      // 38
#define AR_SYMB 9
  {CWTREE_UNKNOWN_SIG, 55, AR_IDX},      // 39 <ar>
  {56, CWTREE_UNKNOWN_SIG,NOT_IN_POOL_IDX},      // 40
  {57, CWTREE_UNKNOWN_SIG,A2IDX('1')},      // 41
  {CWTREE_UNKNOWN_SIG, 58,A2IDX('6')},      // 42
#define BT_SYMB 10
  {67, CWTREE_UNKNOWN_SIG,BT_IDX},      // 43
  {CWTREE_UNKNOWN_SIG, CWTREE_UNKNOWN_SIG,SLASH_IDX},      // 44
#define KA_SYMB 4
  {59, 60,KA_IDX},      // 45 !! <ka>
#define KN_SYMB 5
  {CWTREE_UNKNOWN_SIG, 63,KN_IDX},      // 46   !! <kn>
  {CWTREE_UNKNOWN_SIG, CWTREE_UNKNOWN_SIG,A2IDX('7')},      // 47
  {CWTREE_UNKNOWN_SIG, 61, NOT_IN_POOL_IDX},      // 48
  {62, CWTREE_UNKNOWN_SIG,A2IDX('8')},      // 49
  {CWTREE_UNKNOWN_SIG, CWTREE_UNKNOWN_SIG,A2IDX('9')},      // 50
  {CWTREE_UNKNOWN_SIG, CWTREE_UNKNOWN_SIG,A2IDX('0')},      // 51
  //
#define SK_SYMB 6
  {CWTREE_UNKNOWN_SIG, CWTREE_UNKNOWN_SIG, SK_IDX},      // 52 !! <sk>
  {CWTREE_UNKNOWN_SIG, CWTREE_UNKNOWN_SIG, QUESTION_IDX},      // 53
  {CWTREE_UNKNOWN_SIG, CWTREE_UNKNOWN_SIG, NOT_IN_POOL_IDX},      // 54
  {CWTREE_UNKNOWN_SIG, CWTREE_UNKNOWN_SIG, POINT_IDX},      // 55
  {CWTREE_UNKNOWN_SIG, CWTREE_UNKNOWN_SIG, AT_IDX},      // 56
  {CWTREE_UNKNOWN_SIG, CWTREE_UNKNOWN_SIG, NOT_IN_POOL_IDX},     // 57
  {CWTREE_UNKNOWN_SIG, CWTREE_UNKNOWN_SIG, MINUS_IDX},      // 58
  {CWTREE_UNKNOWN_SIG, CWTREE_UNKNOWN_SIG, COLON_IDX},  // 59//use SEMICOLON_IDX!
  {CWTREE_UNKNOWN_SIG, CWTREE_UNKNOWN_SIG, NOT_IN_POOL_IDX},      // 60
  {CWTREE_UNKNOWN_SIG, CWTREE_UNKNOWN_SIG, COMMA_IDX},      // 61
  {CWTREE_UNKNOWN_SIG, CWTREE_UNKNOWN_SIG,COLON_IDX},      // 62
#define KK_SYMB 7
  {CWTREE_UNKNOWN_SIG, CWTREE_UNKNOWN_SIG,KK_IDX},      // 63 <kk>
  {65, CWTREE_UNKNOWN_SIG,NOT_IN_POOL_IDX},      // 64
  //
  {66, CWTREE_UNKNOWN_SIG,NOT_IN_POOL_IDX},      // 65
#define HH_SYMB 8
  {CWTREE_UNKNOWN_SIG, CWTREE_UNKNOWN_SIG,HH_IDX},      // 66 <hh>
  //
  {CWTREE_UNKNOWN_SIG, 68,NOT_IN_POOL_IDX},// 67
#define BK_SYMB 11
  {CWTREE_UNKNOWN_SIG, CWTREE_UNKNOWN_SIG,BK_IDX},// <bk> 68
  {CWTREE_UNKNOWN_SIG, CWTREE_UNKNOWN_SIG,NOT_IN_POOL_IDX},// 69 Default
                                                      // for all
                                                      // unidentified
                                                      // characters
  {CWTREE_UNKNOWN_SIG, CWTREE_UNKNOWN_SIG,BLANK_IDX}// 70 Just to be able to
                                                    //point the treeptr to a blank
};

// pointer used to navigate within the linked list representing the
// dichotomic tree
byte treeptr = CWTREE_ROOT;
// the string that results from decoding one sign
char sigString[5];


//// special characters are: ch, <as>, <ka>, <kn>, <sk>
//                           01   02    03    04    05
//                           53   54    55    56    57
// Alpha        a
// Numerisch    9
// Interpunkt   . , : - / = ? @ +    010101 110011 111000 100001 10010 10001 001100 011010 01010
// Betriebsabk  <>  <as> <ka> <kn> <sk>
// German       ä
// Grouping:   a    |  9   |  .   |  <>   |  a9  | .<>   |   äa    | äa9   | a9.   | a9.<> | äa9.<>
//            4-29  | 30-39| 40-48| 48-52 | 4-39 | 40-52 | 0-29    | 0-39  | 4-48  | 4-52  | 0-52


// timer to detect interword spaces
unsigned long interWordTimer = 0;
// timer to use for automatic character spacing (ACS)
unsigned long acsTimer = 0;

/////// variables for morse code trainer mode

// each morse sign consists of elements (dits or dahs).
// number of elements this sign has:
byte sig_number_of_cw_elems = 0;
// the list of elements; 0 = dit, 1 = dah
byte sig_cw_elems[8];

unsigned char generatorState, generatorMode;
// need to be properly initialised:
unsigned int interCharacterSpace, interWordSpace;
unsigned long timer = 0;
uint8_t pointer = 0;
uint8_t startPool, endPool, kochLevel;
// to indicate that we are starting a new sequence in trainer mode
boolean doStartSequence = false;
#define START_SEQ_LENGTH 6
byte startSequence[] = {A2IDX('v'), A2IDX('v'),A2IDX('v'), KA_IDX,
                        BLANK_IDX, BLANK_IDX};
// index used for going through the startSequence
byte k_startSeq;
// when entering a Koch training level, all the new characters will be
// slowly demonstrated n times
#define KOCH_START_SEQ_N 3

// flag for trainer mode. In trainer mode, the generator can be active
// or paused.
boolean active = false;

// whether to send training characters to the external world
boolean keyTrainerMode = false;



/// strings for q-groups and abbreviations to be stored in PROGMEM
const char abbrev0[] PROGMEM =  "33";
const char abbrev1[] PROGMEM =  "44";
const char abbrev2[] PROGMEM =  "55";
const char abbrev3[] PROGMEM =  "72";
const char abbrev4[] PROGMEM =  "73";
const char abbrev5[] PROGMEM =  "88";
const char abbrev6[] PROGMEM =  "99";
const char abbrev7[] PROGMEM =  "aa";
const char abbrev8[] PROGMEM =  "ab";
const char abbrev9[] PROGMEM =  "abt";
const char abbrev10[] PROGMEM =  "best";
const char abbrev11[] PROGMEM =  "adr";
const char abbrev12[] PROGMEM =  "all";
const char abbrev13[] PROGMEM =  "age";
const char abbrev14[] PROGMEM =  "agn";
const char abbrev15[] PROGMEM =  "dipole";
const char abbrev16[] PROGMEM =  "am";
const char abbrev17[] PROGMEM =  "coil";
const char abbrev18[] PROGMEM =  "ans";
const char abbrev19[] PROGMEM =  "ant";
const char abbrev20[] PROGMEM =  "atv";
const char abbrev21[] PROGMEM =  "agbp";
const char abbrev22[] PROGMEM =  "award";
const char abbrev23[] PROGMEM =  "awdh";
const char abbrev24[] PROGMEM =  "awds";
const char abbrev25[] PROGMEM =  "bc";
const char abbrev26[] PROGMEM =  "bci";
const char abbrev27[] PROGMEM =  "bcnu";
const char abbrev28[] PROGMEM =  "bd";
const char abbrev29[] PROGMEM =  "bureau";
const char abbrev30[] PROGMEM =  "cldy";//cloudy
const char abbrev31[] PROGMEM =  "bpm";
const char abbrev32[] PROGMEM =  "btr";
const char abbrev33[] PROGMEM =  "btw";
const char abbrev34[] PROGMEM =  "bug";
const char abbrev35[] PROGMEM =  "buro";
const char abbrev36[] PROGMEM =  "b4";
const char abbrev37[] PROGMEM =  "call";
const char abbrev38[] PROGMEM =  "cfm";
const char abbrev39[] PROGMEM =  "cl";
const char abbrev40[] PROGMEM =  "conds";
const char abbrev41[] PROGMEM =  "condx";
const char abbrev42[] PROGMEM =  "congrats";
const char abbrev43[] PROGMEM =  "cq";
const char abbrev44[] PROGMEM =  "cu";
const char abbrev45[] PROGMEM =  "cuagn";
const char abbrev46[] PROGMEM =  "cul";
const char abbrev47[] PROGMEM =  "cw";
const char abbrev48[] PROGMEM =  "db";
const char abbrev49[] PROGMEM =  "sunny";
const char abbrev50[] PROGMEM =  "de";
const char abbrev51[] PROGMEM =  "diff";
const char abbrev52[] PROGMEM =  "dr";
const char abbrev53[] PROGMEM =  "dwn";
const char abbrev54[] PROGMEM =  "dx";
const char abbrev55[] PROGMEM =  "ee";
const char abbrev56[] PROGMEM =  "el";
const char abbrev57[] PROGMEM =  "elbug";
const char abbrev58[] PROGMEM =  "es";
const char abbrev59[] PROGMEM =  "excus";
const char abbrev60[] PROGMEM =  "fb";
const char abbrev61[] PROGMEM =  "fer";
const char abbrev62[] PROGMEM =  "fm";
const char abbrev63[] PROGMEM =  "fone";
const char abbrev64[] PROGMEM =  "fr";
const char abbrev65[] PROGMEM =  "frd";
const char abbrev66[] PROGMEM =  "freq";
const char abbrev67[] PROGMEM =  "fwd";
const char abbrev68[] PROGMEM =  "ga";
const char abbrev69[] PROGMEM =  "gb";
const char abbrev70[] PROGMEM =  "c";
const char abbrev71[] PROGMEM =  "gd";
const char abbrev72[] PROGMEM =  "ge";
const char abbrev73[] PROGMEM =  "gl";
const char abbrev74[] PROGMEM =  "gm";
const char abbrev75[] PROGMEM =  "gn";
const char abbrev76[] PROGMEM =  "gnd";
const char abbrev77[] PROGMEM =  "gp";
const char abbrev78[] PROGMEM =  "gs";
const char abbrev79[] PROGMEM =  "gt";
const char abbrev80[] PROGMEM =  "gud";
const char abbrev81[] PROGMEM =  "ham";
const char abbrev82[] PROGMEM =  "w3dzz";
const char abbrev83[] PROGMEM =  "hi";
const char abbrev84[] PROGMEM =  "hpe";
const char abbrev85[] PROGMEM =  "hr";
const char abbrev86[] PROGMEM =  "hrd";
const char abbrev87[] PROGMEM =  "hrs";
const char abbrev88[] PROGMEM =  "hv";
const char abbrev89[] PROGMEM =  "hvy";
const char abbrev90[] PROGMEM =  "hw";
const char abbrev91[] PROGMEM =  "is";
const char abbrev92[] PROGMEM =  "g5rv";
const char abbrev93[] PROGMEM =  "in";
const char abbrev94[] PROGMEM =  "ii";
const char abbrev95[] PROGMEM =  "info";
const char abbrev96[] PROGMEM =  "inpt";
const char abbrev97[] PROGMEM =  "irc";
const char abbrev98[] PROGMEM =  "zepp";
const char abbrev99[] PROGMEM =  "k";
const char abbrev100[] PROGMEM =  "khz";
const char abbrev101[] PROGMEM =  "km";
const char abbrev102[] PROGMEM =  "kw";
const char abbrev103[] PROGMEM =  "ky";
const char abbrev104[] PROGMEM =  "lbr";
const char abbrev105[] PROGMEM =  "windom";
const char abbrev106[] PROGMEM =  "lid";
const char abbrev107[] PROGMEM =  "lis";
const char abbrev108[] PROGMEM =  "lng";
const char abbrev109[] PROGMEM =  "log";
const char abbrev110[] PROGMEM =  "lp";
const char abbrev111[] PROGMEM =  "rain";
const char abbrev112[] PROGMEM =  "luf";
const char abbrev113[] PROGMEM =  "lw";
const char abbrev114[] PROGMEM =  "on";
const char abbrev115[] PROGMEM =  "mesz";
const char abbrev116[] PROGMEM =  "mez";
const char abbrev117[] PROGMEM =  "mgr";
const char abbrev118[] PROGMEM =  "mhz";
const char abbrev119[] PROGMEM =  "min";
const char abbrev120[] PROGMEM =  "not";
const char abbrev121[] PROGMEM =  "mni";
const char abbrev122[] PROGMEM =  "mod";
const char abbrev123[] PROGMEM =  "msg";
const char abbrev124[] PROGMEM =  "mtr";
const char abbrev125[] PROGMEM =  "muf";
const char abbrev126[] PROGMEM =  "my";
const char abbrev127[] PROGMEM =  "n";
const char abbrev128[] PROGMEM =  "net";
const char abbrev129[] PROGMEM =  "magloop";
const char abbrev130[] PROGMEM =  "nil";
const char abbrev131[] PROGMEM =  "no";
const char abbrev132[] PROGMEM =  "nice";
const char abbrev133[] PROGMEM =  "nr";
const char abbrev134[] PROGMEM =  "nw";
const char abbrev135[] PROGMEM =  "ok";
const char abbrev136[] PROGMEM =  "om";
const char abbrev137[] PROGMEM =  "op";
const char abbrev138[] PROGMEM =  "osc";
const char abbrev139[] PROGMEM =  "oscar";
const char abbrev140[] PROGMEM =  "output";
const char abbrev141[] PROGMEM =  "ow";
const char abbrev142[] PROGMEM =  "pa";
const char abbrev143[] PROGMEM =  "pep";
const char abbrev144[] PROGMEM =  "pm";
const char abbrev145[] PROGMEM =  "pse";
const char abbrev146[] PROGMEM =  "psed";
const char abbrev147[] PROGMEM =  "pwr";
const char abbrev148[] PROGMEM =  "px";
const char abbrev149[] PROGMEM =  "qra";
const char abbrev150[] PROGMEM =  "qrb";
const char abbrev151[] PROGMEM =  "qrg";
const char abbrev152[] PROGMEM =  "qrl?";
const char abbrev153[] PROGMEM =  "qrm";
const char abbrev154[] PROGMEM =  "qrn";
const char abbrev155[] PROGMEM =  "qro";
const char abbrev156[] PROGMEM =  "qrp";
const char abbrev157[] PROGMEM =  "qrq";
const char abbrev158[] PROGMEM =  "qrs";
const char abbrev159[] PROGMEM =  "qrt";
const char abbrev160[] PROGMEM =  "qru";
const char abbrev161[] PROGMEM =  "qrv";
const char abbrev162[] PROGMEM =  "qrx";
const char abbrev163[] PROGMEM =  "qrz";
const char abbrev164[] PROGMEM =  "qsb";
const char abbrev165[] PROGMEM =  "qsk";
const char abbrev166[] PROGMEM =  "qsl";
const char abbrev167[] PROGMEM =  "qso";
const char abbrev168[] PROGMEM =  "qsp";
const char abbrev169[] PROGMEM =  "qst";
const char abbrev170[] PROGMEM =  "qsy";
const char abbrev171[] PROGMEM =  "qtc";
const char abbrev172[] PROGMEM =  "qth";
const char abbrev173[] PROGMEM =  "qtr";
const char abbrev174[] PROGMEM =  "r";
const char abbrev175[] PROGMEM =  "rcvd";
const char abbrev176[] PROGMEM =  "re";
const char abbrev177[] PROGMEM =  "ref";
const char abbrev178[] PROGMEM =  "rf";
const char abbrev179[] PROGMEM =  "rfi";
const char abbrev180[] PROGMEM =  "rig";
const char abbrev181[] PROGMEM =  "rpt";
const char abbrev182[] PROGMEM =  "rprt";
const char abbrev183[] PROGMEM =  "rst";
const char abbrev184[] PROGMEM =  "rtty";
const char abbrev185[] PROGMEM =  "rx";
const char abbrev186[] PROGMEM =  "sase";
const char abbrev187[] PROGMEM =  "shf";
const char abbrev188[] PROGMEM =  "sigs";
const char abbrev189[] PROGMEM =  "sked";
const char abbrev190[] PROGMEM =  "sn";
const char abbrev191[] PROGMEM =  "sp";
const char abbrev192[] PROGMEM =  "sri";
const char abbrev193[] PROGMEM =  "ssb";
const char abbrev194[] PROGMEM =  "sstv";
const char abbrev195[] PROGMEM =  "stn";
const char abbrev196[] PROGMEM =  "sure";
const char abbrev197[] PROGMEM =  "swl";
const char abbrev198[] PROGMEM =  "warm";
const char abbrev199[] PROGMEM =  "t";
const char abbrev200[] PROGMEM =  "temp";
const char abbrev201[] PROGMEM =  "test";
const char abbrev202[] PROGMEM =  "tia";
const char abbrev203[] PROGMEM =  "tks";
const char abbrev204[] PROGMEM =  "tnx";
const char abbrev205[] PROGMEM =  "trx";
const char abbrev206[] PROGMEM =  "tu";
const char abbrev207[] PROGMEM =  "tvi";
const char abbrev208[] PROGMEM =  "tx";
const char abbrev209[] PROGMEM =  "u";
const char abbrev210[] PROGMEM =  "ufb";
const char abbrev211[] PROGMEM =  "uhf";
const char abbrev212[] PROGMEM =  "ukw";
const char abbrev213[] PROGMEM =  "unlis";
const char abbrev214[] PROGMEM =  "up";
const char abbrev215[] PROGMEM =  "ur";
const char abbrev216[] PROGMEM =  "sota";
const char abbrev217[] PROGMEM =  "utc";
const char abbrev218[] PROGMEM =  "v";
const char abbrev219[] PROGMEM =  "vert";
const char abbrev220[] PROGMEM =  "vfo";
const char abbrev221[] PROGMEM =  "via";
const char abbrev222[] PROGMEM =  "vl";
const char abbrev223[] PROGMEM =  "vln";
const char abbrev224[] PROGMEM =  "vy";
const char abbrev225[] PROGMEM =  "w";
const char abbrev226[] PROGMEM =  "watts";
const char abbrev227[] PROGMEM =  "wid";
const char abbrev228[] PROGMEM =  "wkd";
const char abbrev229[] PROGMEM =  "wkg";
const char abbrev230[] PROGMEM =  "wl";
const char abbrev231[] PROGMEM =  "snow";
const char abbrev232[] PROGMEM =  "wtts";
const char abbrev233[] PROGMEM =  "wx";
const char abbrev234[] PROGMEM =  "xcus";
const char abbrev235[] PROGMEM =  "xcvr";
const char abbrev236[] PROGMEM =  "what";
const char abbrev237[] PROGMEM =  "yl";
const char abbrev238[] PROGMEM =  "name";
const char abbrev239[] PROGMEM =  "dok";
const char abbrev240[] PROGMEM =  "locator";
const char abbrev241[] PROGMEM =  "yday";
const char abbrev242[] PROGMEM =  "plus";
const char abbrev243[] PROGMEM =  "also";


const byte NUMBER_OF_ABBREVS = 244;
//// these are used in the qsotree:
//liste of elements that is to be worked in order.
#define LIST_SEQ (NUMBER_OF_ABBREVS)
//liste of alternatives. only one is worked (randomly).
#define LIST_ALT (NUMBER_OF_ABBREVS+1)
//list of elements that is to be worked in arbitrary order.
#define LIST_MIX (NUMBER_OF_ABBREVS+2)
//one element list, which is worked only with a probability (length value is expectde
//between 0 and 100 and means probability) or skipped otherwise.
#define PROB (NUMBER_OF_ABBREVS+3)
//one element list, whose element value is not interpreted as an abbrev index,
//but can be used to escape magic, to generate calls, groups, numbers, etc.
// the length value is currently ignored.
#define ESC_SPECIAL (NUMBER_OF_ABBREVS+4)
// these are to be used together with ESC_SPECIAL:
#define REPEAT_MAGIC 0
#define PAUSE_MAGIC 1
#define CALL1_MAGIC 2
#define CALL2_MAGIC 3
#define RANDCALL_MAGIC 4
#define RST_MAGIC 5
#define NUM1_MAGIC 6
#define NUM2_MAGIC 7
#define NUM3_MAGIC 8
#define GROUPOF5_MAGIC 9
#define LOCATOR_MAGIC 10
#define DOK_MAGIC 11
#define NAME1_MAGIC 12
#define NAME2_MAGIC 13
#define SOTA_MAGIC 14

const byte ABBREV_MAX_SIZE = 9;

PGM_P const abbreviations[NUMBER_OF_ABBREVS] PROGMEM = {
  abbrev0, abbrev1, abbrev2, abbrev3, abbrev4, abbrev5, abbrev6, abbrev7, abbrev8, abbrev9, abbrev10,
  abbrev11, abbrev12, abbrev13, abbrev14, abbrev15, abbrev16, abbrev17, abbrev18, abbrev19, abbrev20,
  abbrev21, abbrev22, abbrev23, abbrev24, abbrev25, abbrev26, abbrev27, abbrev28, abbrev29, abbrev30,
  abbrev31, abbrev32, abbrev33, abbrev34, abbrev35, abbrev36, abbrev37, abbrev38, abbrev39, abbrev40,
  abbrev41, abbrev42, abbrev43, abbrev44, abbrev45, abbrev46, abbrev47, abbrev48, abbrev49, abbrev50,
  abbrev51, abbrev52, abbrev53, abbrev54, abbrev55, abbrev56, abbrev57, abbrev58, abbrev59, abbrev60,
  abbrev61, abbrev62, abbrev63, abbrev64, abbrev65, abbrev66, abbrev67, abbrev68, abbrev69, abbrev70,
  abbrev71, abbrev72, abbrev73, abbrev74, abbrev75, abbrev76, abbrev77, abbrev78, abbrev79, abbrev80,
  abbrev81, abbrev82, abbrev83, abbrev84, abbrev85, abbrev86, abbrev87, abbrev88, abbrev89, abbrev90,
  abbrev91, abbrev92, abbrev93, abbrev94, abbrev95, abbrev96, abbrev97, abbrev98, abbrev99, abbrev100,
  abbrev101, abbrev102, abbrev103, abbrev104, abbrev105, abbrev106, abbrev107, abbrev108, abbrev109, abbrev110,
  abbrev111, abbrev112, abbrev113, abbrev114, abbrev115, abbrev116, abbrev117, abbrev118, abbrev119, abbrev120,
  abbrev121, abbrev122, abbrev123, abbrev124, abbrev125, abbrev126, abbrev127, abbrev128, abbrev129, abbrev130,
  abbrev131, abbrev132, abbrev133, abbrev134, abbrev135, abbrev136, abbrev137, abbrev138, abbrev139, abbrev140,
  abbrev141, abbrev142, abbrev143, abbrev144, abbrev145, abbrev146, abbrev147, abbrev148, abbrev149, abbrev150,
  abbrev151, abbrev152, abbrev153, abbrev154, abbrev155, abbrev156, abbrev157, abbrev158, abbrev159, abbrev160,
  abbrev161, abbrev162, abbrev163, abbrev164, abbrev165, abbrev166, abbrev167, abbrev168, abbrev169, abbrev170,
  abbrev171, abbrev172, abbrev173, abbrev174, abbrev175, abbrev176, abbrev177, abbrev178, abbrev179, abbrev180,
  abbrev181, abbrev182, abbrev183, abbrev184, abbrev185, abbrev186, abbrev187, abbrev188, abbrev189, abbrev190,
  abbrev191, abbrev192, abbrev193, abbrev194, abbrev195, abbrev196, abbrev197, abbrev198, abbrev199, abbrev200,
  abbrev201, abbrev202, abbrev203, abbrev204, abbrev205, abbrev206, abbrev207, abbrev208, abbrev209, abbrev210,
  abbrev211, abbrev212, abbrev213, abbrev214, abbrev215, abbrev216, abbrev217, abbrev218, abbrev219, abbrev220,
  abbrev221, abbrev222, abbrev223, abbrev224, abbrev225, abbrev226, abbrev227, abbrev228, abbrev229, abbrev230,
  abbrev231, abbrev232, abbrev233, abbrev234, abbrev235, abbrev236, abbrev237, abbrev238, abbrev239, abbrev240,
  abbrev241, abbrev242, abbrev243
};

//Teaching order of the signs:
const char kochOrder[] = {
  // This sign order is basically the same as DJ4UF uses for his
  // course and book.
  A2IDX('e'), A2IDX('s'), A2IDX('n'),
  A2IDX('o'), HH_IDX, A2IDX('t'),
  A2IDX('q'), A2IDX('5'), A2IDX('r'),
  A2IDX('u'), A2IDX('9'), BT_IDX,
  A2IDX('c'), A2IDX('d'), A2IDX('0'),
  A2IDX('a'), A2IDX('l'), A2IDX('8'),
  A2IDX('i'), A2IDX('x'), A2IDX('4'),
  POINT_IDX,  A2IDX('m'), A2IDX('7'),
  A2IDX('y'), A2IDX('1'), SLASH_IDX,
  A2IDX('h'), A2IDX('6'), A2IDX('m'),
  A2IDX('7'), QUESTION_IDX, A2IDX('j'),
  A2IDX('3'), A2IDX('g'), AS_IDX,
  A2IDX('2'), A2IDX('v'), COMMA_IDX,
  A2IDX('k'), A2IDX('z'), AR_IDX,
  A2IDX('f'), A2IDX('b'), KN_IDX,
  A2IDX('p'), A2IDX('w'), MINUS_IDX,
  SK_IDX, VE_IDX, BK_IDX,
  AT_IDX, KK_IDX, COLON_IDX
};
#define KOCH_ORDER_SIZE (18*3)
// this does not have to be a divisor of KOCH_ORDER_SIZE:
#define KOCH_SIGNS_PER_LEVEL 3

#define GENERATOR_MODE_N_KOCH_LEVELS (KOCH_ORDER_SIZE / KOCH_SIGNS_PER_LEVEL +\
  (KOCH_ORDER_SIZE % KOCH_SIGNS_PER_LEVEL ? 1 : 0))



// hostprefix,slash,prefix,number,suffix,slash,addition, blank
#define CALLSIGN_MAX_SIZE (2+1+2+1+3+1+3+1)

// the array to store the sigs of the word that the generator has spit
// out and which is being given sig by sig right now. +1 for END_OF_WORD_IDX
#define MAX_SIG_WORD_LENGTH max(ABBREV_MAX_SIZE+1, CALLSIGN_MAX_SIZE+1)
byte current_sig_word[MAX_SIG_WORD_LENGTH];
// the corresponding index with respect to current_sig_word
byte icurrent_sig;
// the buffer qso_callsign1 is also misused to hold the signs the
// CHOICE generator
byte qso_callsign1[MAX_SIG_WORD_LENGTH];
byte qso_callsign2[MAX_SIG_WORD_LENGTH];
// the qso_name buffers are also misused for the quickEcho game
byte qso_name1[ABBREV_MAX_SIZE+1];
byte qso_name2[ABBREV_MAX_SIZE+1];

#ifdef ENABLE_QSOTEXT_GENERATOR
/* root list of qsotree is a seq list. */
/* each element of a list can be an element or any other kind of list. */
// this tree contains abbrev indices, i.e. indices with respect to the
// abbreviations array
#include "qsotree.h"
uint8_t mixHistoryLength;

int16_t iqso;
uint16_t currentlist_start;
#ifdef SIMULATE_MISTAKES
int16_t last_iqso;
boolean mistakes_enabled;
enum __attribute__ ((__packed__)) whichQSOElemType {next=0, corrected, last, nextAndThisNoMistake, thisNoMistake};
whichQSOElemType whichQSOElem;
#endif

boolean nextQSOElem(boolean save_iqso){
  // put the iqso pointer onto the next element. This element can be
  // either another element of the current list, or an element of
  // another list (parent, sibling, etc. any relation is possible)  list.
#ifdef SIMULATE_MISTAKES
  if(whichQSOElem == last) {
    whichQSOElem = corrected;
    evalQSOElem(last_iqso);
    return true;
  }else if(whichQSOElem == corrected) {
    whichQSOElem = thisNoMistake;
    evalQSOElem(iqso);
    return true;
  }
  // first of all, remember the current element
  if(save_iqso) last_iqso = iqso;
#endif
  byte currentlist_start_byte, iqso_byte;

  boolean list_finished = true;
  while(list_finished) {
    // first check if the current list has any elements left.
    currentlist_start_byte = pgm_read_byte_near(&(qsotree[currentlist_start]));

    if(currentlist_start_byte == LIST_SEQ || currentlist_start_byte == ESC_SPECIAL) {
      // have we just finished the current list?
      list_finished = (iqso == getListEnd(currentlist_start));
    }else if(currentlist_start_byte == LIST_ALT) {
      list_finished = true;
    }else if(currentlist_start_byte == LIST_MIX) {
      list_finished = true;
      for (uint16_t n = 0; n < getListLen(currentlist_start); n++){
        if(!mixHistoryContains(getListElemStartIndex(currentlist_start, n))){
          list_finished = false;
          break;
        }
      }
      // if we wanted to use the real minimum history length we would use the follow line.
      // But this adds more code bytes to the rom than the simpler method adds data bytes!
      //if(list_finished) mixHistoryLength -= getListLen(currentlist_start);
    }else if(currentlist_start_byte == PROB) {
      list_finished = true;
    }

    if(list_finished) {
      // go to end of this list.
      iqso = getListEnd(currentlist_start);
      // select parent list
      uint16_t liststart = getListStart(currentlist_start);
      // the getListStart function returns a value larger than its
      // argument, if it thinks this is the qso end.
      if(liststart > currentlist_start)
        // end of qso
        return false;
      else
        currentlist_start = liststart;
      // and go into a new cycle..
    }
  }

  // ok, so we have found the parent list that is still unfinished at
  // this place. currentlist_start and currentlist_start_byte are
  // valid for this list.  it can be a seq or a mix list.  now go to its
  // next element!

  if(currentlist_start_byte == LIST_SEQ || currentlist_start_byte == ESC_SPECIAL){
    if(iqso == currentlist_start)
      iqso += 2;
    else
      iqso++;
  }else if(currentlist_start_byte == LIST_MIX){
    // there must be at least one left, we have already checked that
    // above.
    iqso = getListElemStartIndex(currentlist_start,
                                   random(0, getListLen(currentlist_start)));
    while(mixHistoryContains(iqso))
      iqso = getListElemStartIndex(currentlist_start,
                                   random(0, getListLen(currentlist_start)));
    // now iqso is an element of this list which has not been used
    // yet. record it, in order not to choose it any more.
    mixHistory[mixHistoryLength++] = iqso;
  }

  // now this element could be just an element, or a list start.
  // we cycle until we get a regular element
  while(true){
    iqso_byte = pgm_read_byte_near(&(qsotree[iqso]));
    if(isListStart(iqso_byte)){
      // this is a list start!
      currentlist_start = iqso;

      //we have to get the next element of that list, instead of
      //returning the list start element.
      if(iqso_byte == LIST_SEQ || iqso_byte == ESC_SPECIAL) {
        // the following byte is the list length.
        // so skip that one and go to the one after.
        iqso += 2;
      }else if(iqso_byte == LIST_ALT) {
        iqso = getListElemStartIndex(currentlist_start,
                                     random(0, getListLen(iqso)));
      }else if(iqso_byte == LIST_MIX) {
        do{
        iqso = getListElemStartIndex(currentlist_start,
                                     random(0, getListLen(iqso)));
        }while(mixHistoryContains(iqso));
        // now iqso is an element of this list which has not been used
        // yet. record it, in order not to choose it any more.
        mixHistory[mixHistoryLength++] = iqso;
      }else if(iqso_byte == PROB) {
        iqso = currentlist_start + 2;
        if((byte)pgm_read_byte_near(&(qsotree[currentlist_start + 1])) < random(0, 100)) {
          // if the dice say we dont want this element then recurse to
          // get a new one
          iqso = getListEnd(currentlist_start);

          return nextQSOElem(false);
        }
      }
    }else{
      // just a normal element, indicating an abbrev or something special.
      break;
    }
  }

  evalQSOElem(iqso);
  return true;
}

boolean mixHistoryContains(uint16_t iqso){
  for(uint16_t i = 0; i < mixHistoryLength; i++){
    if(mixHistory[i] == iqso) return true;
  }
  return false;
}

boolean isListStart(byte iqso_byte){
  return iqso_byte == LIST_SEQ ||
    iqso_byte == LIST_ALT ||
    iqso_byte == LIST_MIX ||
    iqso_byte == PROB ||
    iqso_byte == ESC_SPECIAL;
}
boolean isEscSpecial(byte iqso_byte){
  // this is a special list.
  return iqso_byte == ESC_SPECIAL;
}

uint16_t getListLen(uint16_t list_start){
  return pgm_read_byte_near(&(qsotree[list_start])) == PROB ?
    1 : pgm_read_byte_near(&(qsotree[list_start + 1]));
}

uint16_t getListStart(uint16_t iqso){
  // hence, search backwards for the innermost list (i.e. the first one encounters)
  // that is still open at this iqso position.
  uint16_t sum_contained_lists_len = 0;
  byte jqso_byte;
  if(iqso == 0){
    // if iqso=0 is given (which is the root liststart) then there is
    // no further parent list. (-> end of qso.)

    // return something to indicate end-of-qso: a number > iqso is
    // surely not a valid answer in any other case, so use this.
    return iqso+1;
  }
  for(int16_t jqso = iqso-1; jqso >= 0; jqso--) {
    jqso_byte = pgm_read_byte_near(&(qsotree[jqso]));
    if(isListStart(jqso_byte)) {
      // now check if iqso is still part of this list.

      // check current jqso + elements of this list + number of elements of all contained lists:
      if(iqso <= jqso + 1+getListLen(jqso) + sum_contained_lists_len){
        //the given iqso element is part of that list.
        return jqso;
      }else{
        // this list has ended before the iqso given as parameter.
        sum_contained_lists_len += 1+getListLen(jqso);
      }
    }
    if(jqso == 0){
      // if jqso==0 (which is the root liststart) has not triggered the
      // return above then there is no open list left. end of qso.

      // return something to indicate end-of-qso: a number > iqso is
      // surely not a valid answer in any other case, so use this.
      return iqso+1;
    }
  }
}

uint16_t getListEnd(uint16_t list_start){
  // this function returns the index of the last element that still belongs to
  // this list, wrt qsotree.
  return getListElemEndIndex(list_start, getListLen(list_start)-1);
}
uint16_t getListElemEndIndex(uint16_t list_start, uint16_t n){
  // this function returns the end index of the nth element of this list,
  // wrt qsotree.
  // n is zero-based.

  // i is the index wrt qsotree.
  // j is element number wrt to the current list.
  uint16_t i = list_start+1;
  for(uint16_t j = 0;
      j < n+1;
      j++) {
    i++;
    if(isListStart(pgm_read_byte_near(&(qsotree[i]))))
      // if this is a list start, jump to the end of that list.
      i = getListEnd(i);
  }
  return i;
}
uint16_t getListElemStartIndex(uint16_t list_start, uint16_t n){
  // this function returns the start index of the nth element of this list,
  // wrt qsotree.
  // n is zero-based.

  // i is the index wrt qsotree.
  // j is element number wrt to the current list.
  uint16_t i = list_start+2;
  for(uint16_t j = 0;
      j < n;
      j++) {
    if(isListStart(pgm_read_byte_near(&(qsotree[i]))))
      // if this is a list start, jump to the end of that list.
      i = getListEnd(i);
    i++;
  }
  return i;
}

void initNewQSO(){
  // clear the list where we note which mix elements have been printed
  // already.
  mixHistoryLength = 0;

  iqso = -1;
  currentlist_start = 0;
#ifdef SIMULATE_MISTAKES
  last_iqso = -1;
  whichQSOElem = thisNoMistake;
#endif

  generateCallsign();
  memcpy(qso_callsign1, current_sig_word, MAX_SIG_WORD_LENGTH);
  generateCallsign();
  memcpy(qso_callsign2, current_sig_word, MAX_SIG_WORD_LENGTH);
  generateGroupOf5();
  memcpy(qso_name1, current_sig_word, 7);
  generateGroupOf5();
  memcpy(qso_name2, current_sig_word, 7);
}
#endif

//////////////////////////////////////////////////////////////////////////////
//
//   State Machine Defines

enum __attribute__ ((__packed__)) MORSE_TYPE {KEY_DOWN=0, KEY_UP };
enum __attribute__ ((__packed__)) GEN_TYPE { GROUPOF5=0, CALLSIGNS,  ABBREVS, CHOICE,
#ifdef ENABLE_QSOTEXT_GENERATOR
QSOTEXT,
#endif
    KOCH
#ifdef ENABLE_TEST_GENERATOR
                , TEST_ALL_SIGNS
#endif
#ifdef ENABLE_MEMO_KEYER
    // this one is not available in the CW Trainer
    , MEMO_KEYER_TEXT
#endif
    };

/// variables for morse decoder
///////////////////////////////

// when state is set by manual key or touch paddle, then true!
boolean keyTx = false;
// we use this to decide if Tx should be keyed or not

/// state machine for decoding CW
enum __attribute__ ((__packed__)) DECODER_STATES  {LOW_=0, HIGH_, INTERELEMENT_, INTERCHAR_};
DECODER_STATES decoderState = LOW_;


///////////////////////////////////////////////////////////
// The sampling frq will be 8928 on a 16 mhz
// without any prescaler etc.
const float sampling_freq = 8928.0; // = 2**5 * 3**2 * 31

// Because we need the tone (=target_freq) in the center of the bins
// you can set the target_freq to 496, 558, 744 or 992.
// Anyway, your target freq must be an integer factor
// of the sampling frequency.
const float target_freq = 744.0; /// adjust for your needs see above

// The integer n is the block size, i.e. the number of samples of an
// equivalent FFT.  n also determines the bandwidth.  To make the
// target_freq centered in the bin, n is to be chosen as an multiple
// of the ratio (sampling_freq / target_freq) example: sampling_freq /
// target_freq = 8928/558 = 16 -> choose n = 16*4 = 64 samples
//
// Large n means it takes longer to fill the bin with measured samples and thus
// signals need to be longer to be detected.
//
// try to take n = 96 or 128 ;o)
// 48 will give you a bandwidth around 186 hz
// 64 will give you a bandwidth around 140 hz
// 96 will give you a bandwidth around 94 hz
// 128 will give you a bandwidth around 70 hz
const unsigned int nBuf = ((unsigned int)sampling_freq / target_freq) * 5;

// To detect the morse signal, we try to find a suitable magnitude
// threshold automatically.  To have some inertia, this method employs
// a moving average.  Higher value means the moving average will
// consider older values less -> will follow faster:
#define MAGNITUDE_AUTO_ADJUST_N 6

// A factor, when it comes to detecting state transitions in the morse tone,
// just to have some space up.
#define MAGNITUDE_SAFETY_FACTOR (0.6*0.6)

///////////////////////////////////////////////////////////

// a precomputed coefficient used by Goertzel's algorithm
float coeff;
// a buffer for analog audio data
int audioBuf[nBuf];

//////////////////////////////
// Noise Blanker time which //
// shall be computed?? so     //
// this is initial          //
//////////////////////////////
const byte noiseBlankerTime = 6;  /// ms noise blanker

unsigned long startTimeHigh;
unsigned long highDuration;
long startTimeLow;
long lowDuration;
boolean stop = false;
boolean speedChanged = true;
/// average values of dit and dah lengths to decode as dit or dah and
/// to adapt to speed change:
unsigned long ditAvg, dahAvg;
boolean filteredState = false;
boolean filteredStateBefore = false;

int addressSignature, addressCWsettings;
#ifdef ENABLE_MEMO_KEYER
int addressMemoText;
#endif

void setup() {

#ifdef DEBUG
  // only for debugging purposes! comment out for production code!
  Serial.begin(9600);
#endif

#ifdef ENABLE_USB_KEYBOARD_OUTPUT
  Keyboard.begin();
#endif

  textBuffer[0] = '\0';

  // read what is stored in EEPROM:
  //   signature  byte (to see if we actually have values we
  //                    can meaningfully read - otherwise we
  //                    use the defaults)
  //   CWsettings struct CWs CWsettings (speed, polarity,  mode etc)

  // calculate addresses
#ifdef ENABLE_MEMO_KEYER
  EEPROM.setMemPool(0, E2END);
#endif
  addressSignature = EEPROM.getAddress(sizeof(byte));
  addressCWsettings = EEPROM.getAddress(sizeof(CWsettings));
#ifdef ENABLE_MEMO_KEYER
  addressMemoText = EEPROM.getAddress(MAX_MEMO_TEXT_LENGTH);
#ifdef DEBUG
  /* Serial.println(MAX_MEMO_TEXT_LENGTH); */
  /* Serial.println(addressMemoText); */
#endif
#endif
  if (EEPROM.readByte(addressSignature) == MorserSignature) {
    // OK, we read in values from EEPROM
    EEPROM.readBlock(addressCWsettings, CWsettings);
  }else{
    // otherwise we use the defaults defined in this program

#ifdef ENABLE_MEMO_KEYER
    // erase the Auto Keyer Text
    EEPROM.updateByte(addressMemoText, NOT_IN_POOL_IDX);
#endif
  }
  // first use of device with this version: use defaults, run i2c
  // scanner, write results into EEPROM
  Wire.begin();
  byte i2cAddress = i2cScan();
  if (CWsettings.i2cAddress != i2cAddress) {
    CWsettings.i2cAddress = i2cAddress;
    saveConfig();
  }


  // we reset the LCD address
  // Set the pins on the I2C chip used for LCD connections:
  //                    addr, en,rw,rs,d4,d5,d6,d7,bl,blpol
  lcd = LiquidCrystal_I2C(CWsettings.i2cAddress, 2, 1, 0, 4, 5, 6, 7, 3,
                          POSITIVE);

  // set up the LCD's number of rows and columns:
  lcd.begin(LCD_COLUMNS, LCD_ROWS);

  // create glyphs for volume display
  byte volGlyph[8] = {
    0b11111,
    0b11111,
    0b11110,
    0b11110,
    0b11100,
    0b11100,
    0b11000,
    0b11000
  };
  for (uint8_t i = 0; i < 8; i++) {
    if (i > 0)
      // remove horizontal lines from the Glyph, starting at the top
      volGlyph[i - 1] = 0;
    byte volLevel = 8 - i;
    //lcd.createChar(volLevel-1, volGlyph);
    lcd.createChar((byte)(volLevel - 1), volGlyph);
  }

  // we can use the built-in LED to show when the transmitter is being keyed
  pinMode(keyerPin, OUTPUT);

  digitalWrite(keyerPin, LOW);
  pinMode(straightPin, INPUT_PULLUP);
  // the touch library does this itself - we just do it here in case
  // we use the eternal paddles
  pinMode(leftPin, INPUT_PULLUP);
  pinMode(rightPin, INPUT_PULLUP);
  //randomSeed(analogRead(leftPin));

  // set up the encoder
  pinMode(PinCLK,INPUT_PULLUP);
  pinMode(PinDT,INPUT_PULLUP);

#if defined (KY_040_ENCODER)
  // interrupt 1 is always connected to pin 2 on Arduino UNO ///
  // needs to be LOW insted aof CHANGE for the new encoders
  attachInterrupt (1,isr,CHANGE);
#elif defined (STANDARD_ENCODER)
  // interrupt 1 is always connected to pin 2 on Arduino UNO /// needs
  // to be LOW insted aof CHANGE for the new encoders
  attachInterrupt (1,isr,LOW);
#else
#error Encoder undefined ! Read souce code!
#endif

  // Setup button timers (all in milliseconds / ms)
  // (These are default if not set, but changeable for convenience)
  modeButton.debounceTime   = 40;   // Debounce timer in ms
  modeButton.multiclickTime = 275;  // Time limit for multi clicks
  modeButton.longClickTime  = 400; // time until "held-down clicks" register

#ifdef USB_KEYBOARD_OUTPUT
  Keyboard.begin();
#endif


  // calibrate the paddles

  //create reference values to
  refLeft = ADCTouch.read(leftPin, 1000);
  //account for the capacitance of the pad
  refRight = ADCTouch.read(rightPin, 1000);

  if (CWsettings.useExtPaddle) {
    // ext paddle configured, check if it is there
    if (refLeft < 630) {
      CWsettings.useExtPaddle = false;
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print(F("Use Touch Paddle"));
      delay(1000);
      saveConfig();
    }
  } else {
    // touch configured, check for calibration and for external
    // paddle. check for external paddles connected; you need to
    // close at least one of the contacts for that purpose at start-up
    pinMode(leftPin, INPUT_PULLUP);pinMode(rightPin, INPUT_PULLUP);
    if ((analogRead(leftPin) < 50) || (analogRead(rightPin) < 50)) {
      // a mechanical paddle is pressed, so switch to use them
      CWsettings.useExtPaddle = true;
      saveConfig();
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print(F("Use Ext. Paddle"));
      delay(1000);
    } // end check for external paddle

    else if (refLeft > 630 && refRight > 630) {
      // we use touch sensors and detect touch sensor squeeze at
      // start-up: calibration
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print(F("Calibration"));
      CWsettings.tLeft = refLeft; CWsettings.tRight = refRight;
      delay(1000);
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print(F("Release paddles!"));
      delay(1250);
      //create reference values to
      refLeft = ADCTouch.read(leftPin, 1000);
      //account for the capacitance of the pad
      refRight = ADCTouch.read(rightPin, 1000);

      CWsettings.tLeft = (int)((float)(CWsettings.tLeft - refLeft) *
                               SENS_FACTOR);
      CWsettings.tRight = (int)((float)(CWsettings.tRight - refRight) *
                                SENS_FACTOR);
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print(F("off: "));
      lcd.print(refLeft);
      lcd.print(' ');
      lcd.print(refRight);

      lcd.setCursor(0,1);

      lcd.print(F("on+: "));
      lcd.print(CWsettings.tLeft);
      lcd.print(' ');
      lcd.print(CWsettings.tRight);
      saveConfig();
      delay(3000);
    } // end calibration mode


  }

  // further initialisations
  encoderDelta = 0;

  if (CWsettings.backlightOn)
    lcd.backlight();
  else
    lcd.noBacklight();

  // display startup screen
  clearAndPrintScreen(F("MetaMorserino"),
                      F("V " metaVERSION "  (oe1wkl)"));
  delay(1200);
  lcd.clear();

#ifdef VOLUME2
  if(sidetonePin == 9)
    vol.alternatePin(false);
  else if(sidetonePin == 10)
    vol.alternatePin(true);
#endif

  updateGeneratorMode();

  topMenu();
}


void setupTrainerMode() {
  // erase the textBuffer
  pushChar('\0', false);

  clearAndPrintScreen(F("Start CW Trainer"),
                      F("on/off: Squeeze"));
  active = false;

  reCalcSpeedSetting();

  prepNewGeneratorRun(true);

  secondaryMode = SEC_MODE_GENERATE;

  delay(1000);
  lcd.clear();
  displayTopLine();
}


void setupKeyerMode(const __FlashStringHelper* msg) {

  reCalcSpeedSetting();

  pushChar('\0', false);

  keyerState = IDLE_STATE;

  fullScreenMsg(msg);

  displayTopLine();
}


void setupDecoderMode() {
  encoderState = volumeSettingMode;


  // erase the textBuffer
  pushChar('\0', false);

  fullScreenMsg(F("Start CW Decoder"));
  speedChanged = true;

  displayCWspeed (CWsettings.wpm, true);
  displayVolume();

  /// set up variables for Goertzel Morse Decoder
  setupGoertzel();
  filteredState = filteredStateBefore = false;
  decoderState = LOW_;
  ditAvg = 60;
  dahAvg = 180;
}

void setupCopyGameMode() {
  reCalcSpeedSetting();

  initNewCopyGameRound();
  prepNewGeneratorRun(false);

}
void initNewCopyGameRound() {
  encoderState = scrollMode;

  // erase the textBuffer
  pushChar('\0', false);

  clearCounters();

  secondaryMode = SEC_MODE_GENERATE;
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(F("pse copy this"));
  animateCountdown();
  lcd.clear();
  startCW();
}

void showCopyGameResult(byte shift) {
  // note that At the moment this does not compare the actual CW
  // signs, but this only compares two sections of the textBuffer.
  // This means, instead of keying a sign like 'ch' you can key
  // 'c 'h' and get away with it.
  // however, this would lead to different number of signs in the end.

  int8_t i;
  //actually display the generated text in the top line
  compareCopyGameResult(&i, shift, true);

  while (i < LCD_COLUMNS) {
    // fill the rest of the line with blanks.
    lcd.setCursor(LCD_COLUMNS - 1 - i, 0);
    lcd.print(' ');
    i++;
  }
}

uint8_t compareCopyGameResult(int8_t* i, uint8_t tail_snippet_length, boolean printOnLCD) {
  // this function does either of two closely related things:
  //  a) if printOnLCD==false, it compares the *complete* generated and
  //     keyed text and returns the number of correct signs
  //  b) if printOnLCD==true, it will display either the keyed text in
  //     the upper line, or symbols  indicating correct/wrong signs
  //
  // The textBuffer looks like this:
  // \0...........\0......... >99%
  // i.e. there might be some additional characters after the keyed text.
  // The length of this trailing snippet (here ' >99%', thus 5 characters)
  // must be given in the tail_snippet_length argument.

  char c_keyed, c_generated;
  int16_t j, ic_keyed, ic_generated;
  int16_t correctCount = 0;

  *i = 0;

  while (printOnLCD ? *i < LCD_COLUMNS : true) {
    // either iterate the lcd cells from the right to the left side;
    // or infinite loop, to count all correct symbols (not just those
    // on the display).

    // translate into index of the keyed char.
    j = copyGame_keyedChars - (*i + scrollPosition - tail_snippet_length);

    ic_keyed = textBufferIndex - (*i + scrollPosition);

    // get corresponding keyed char
    c_keyed = textBuffer[textbufModulo(ic_keyed)];

    if (j <= 0)
      // the first sign of keying (most probably) and generator
      // (guaranteed) text is a blank. so if both start with a blank,
      // we would like to not take this into account.

      // FIXME at the moment, the blank at the beginning of the
      // generated signs is forced. this makes for a mistake if the
      // user starts to key very quickly.
      break;

    if (j > copyGame_generatedChars) {
      c_generated = ' ';
    }else{
      ic_generated = textBufferIndex - tail_snippet_length - copyGame_keyedChars -1
        -copyGame_generatedChars + j;
      // -1 , because we have pushed a '\0' after generating signs.
      c_generated = textBuffer[textbufModulo(ic_generated)];
    }

    // put the cursor to the first row of the lcd cell
    lcd.setCursor(LCD_COLUMNS - 1 - *i, 0);

    // if this is a cell which does not belong to the trailing
    // snippet, but to the game input.
    if (*i + scrollPosition >= tail_snippet_length && (c_generated != ' ' || c_keyed != ' ')) {
      if (c_keyed == c_generated) {
        if (printOnLCD) {
          if (showMarks)
            lcd.print(CORRECT_SYMBOL);
          else
            lcd.print(c_generated);
        }
        correctCount++;
      } else if (printOnLCD) {
        if (showMarks)
          lcd.print(MISTAKE_SYMBOL);
        else
          lcd.print(c_generated);
      }
    } else if (printOnLCD) {
      lcd.print(' ');
    }
    *i = *i + 1;
  }
  return correctCount;
}

int16_t trimKeyedChars(int16_t charCount, int16_t genCharCount) {
  // The textBuffer looks like this:
  // \0.................\0...............
  //  |<- gencharCount ->| <-charCount->|
  //    generated string | keyed string

  // This method is supposed to delete blanks in the keyed String.
  // Example: the textBuffer may look like this:
  // \0abc defg hi jklm\0 ab c x xxxx xi j klmxx
  //
  // At the end of this method, the textbuffer should look like this:
  // \0abc defg hi jklm\0abc xxxx xx ijklmxx

  int16_t ic_keyed, ic_generated;
  char c_keyed, c_generated;
  int16_t sum = 0;

  // start at leftmost position.
  for(int16_t i = 0; i < genCharCount; i++) {

    // get corresponding char
    c_generated = textBuffer[textbufModulo(textBufferIndex - charCount - genCharCount + i)];
    ic_keyed = textbufModulo(textBufferIndex - charCount + i + 1);
    c_keyed = textBuffer[ic_keyed];

    if(c_generated == ' '){
      if(c_keyed == ' '){
        continue;
      }else{
        insertCharAt(' ', textbufModulo(textBufferIndex + sum), ic_keyed);
        sum++;
      }
    }else{
      if(c_keyed == ' '){
        //delete this blank in the keyed chars, i.e. shift all
        //following letters 1 position to the left, append a \0 at end
        deleteCharAt(textbufModulo(textBufferIndex + sum), ic_keyed);
        sum--;
      }else{
        continue;
      }
    }
  }

  textBufferIndex = textbufModulo(textBufferIndex + sum);
  return charCount + sum;
}

void deleteCharAt (int16_t textBufferIndex, int16_t ic) {

  for (; ic != textBufferIndex; ic = textbufModulo(ic+1)) {
    textBuffer[ic] = textBuffer[textbufModulo(ic + 1)];
  }
}

void insertCharAt (char c, int16_t textBufferIndex, int16_t ic) {

  // all the characters to the right of position ic must be shifted
  // right by one position.
  // this also shifts the \0 from textBufferIndex+1 to textBufferIndex+2.
  for (int16_t d = textbufModulo(textBufferIndex+1); d != ic; d = textbufModulo(d - 1)) {
    textBuffer[textbufModulo(d + 1)] = textBuffer[d];
  }
  textBuffer[textbufModulo(ic + 1)] = textBuffer[ic];
  textBuffer[ic] = c;
}

#ifdef DEBUG
void serialPrintTextBuffer() {
  int16_t deltastart = -35;
  int16_t deltaend = 10;
  for (deltastart; deltastart <= deltaend; deltastart++) {
    Serial.print(textBuffer[textbufModulo(textBufferIndex + deltastart)] == '\0' ? '0' : textBuffer[textbufModulo(textBufferIndex + deltastart)]);
  }
  Serial.println();
}
#endif


void animateCountdown() {
  for (uint8_t i = COUNTDOWN_SECONDS; i > 0; i--) {
    lcd.setCursor(LCD_COLUMNS / 2, 1);
    lcd.print(i);
    delay(1000);
  }
}

void reCalcSpeedSetting() {
  /*
    After wpm or farnsworth parameters have been changed, we must
    recalculate the dependent quantities.
  */
  CWsettings.wpm = constrain(CWsettings.wpm, 5, 40);
  // set new value for length of dits and dahs
  ditLength = 1200 / CWsettings.wpm;
  dahLength = 3 * ditLength;
  interCharacterSpace = 3 * ditLength * CWsettings.farnsworthMode;
  interWordSpace = morseState == morseQuickEcho ? 0 : 7*ditLength*CWsettings.farnsworthMode;
}


void loop() {
  checkPaddles();
  switch (morseState) {
#ifdef ENABLE_MEMO_KEYER
  case morseSetAutoKeyerText:
    if (doPaddleIambic(leftKey, rightKey))
      // we are busy keying and so need a very tight loop !
      return;
    break;
#endif
  case morseKeyer:
#ifdef ENABLE_MEMO_KEYER
    if (generatorMode == MEMO_KEYER_TEXT) {
      generateCW();
    } else
#endif
    if (doPaddleIambic(leftKey, rightKey))
      // we are busy keying and so need a very tight loop !
      return;
    break;
  case morseTrainer:
    if(generatorMode == CHOICE && secondaryMode ==SEC_MODE_INPUT){
      if (doPaddleIambic(leftKey, rightKey))
        // we are busy keying and so need a very tight loop !
        return;

    }else if(generatorMode == CHOICE && secondaryMode == SEC_MODE_PROCESS) {
        // end of interactive input of sigs.

        // display a message
        fullScreenMsg(F("r mni tnx"));

        prepNewGeneratorRun(morseState == morseTrainer);

        displayTopLine();
        //just to update display
        pushChar(' ', true);

        secondaryMode = SEC_MODE_GENERATE;

    }else if(generatorMode != CHOICE || secondaryMode == SEC_MODE_GENERATE) {
      if (leftKey && rightKey)   {
        // touching both paddles starts and stops the generation of code
        // for debouncing:
        // wait until paddles are released
        while (checkPaddles())
          ;

        if(CWsettings.generatorMode >= GENERATOR_MODE_KOCH_OFFSET) {
          // when entering Koch training, force that
          // the gaps are not prolongated and the
          // speed is a fine end speed.
          CWsettings.wpm = max(18,CWsettings.wpm);

          reCalcSpeedSetting();
          displayTopLine();
        }

        // if there are no sigs in the buffer, then ask the user for
        // input.
        if(qso_callsign1[0] == END_OF_WORD_IDX){
          enterOwnSigsForGenerator();
          break;
        }

        active = !active;
        if(active) {
          // go back to the end of the character stream
          scrollPosition = 0;
          updateDisplayLine();
        }
        // wait for display
        delay(100);

        if (!active)
          vol.noTone(); // stop side tone
        else {
          startCW();
        }
      }
      if (active)
        generateCW();
    }
    break;
  case morseDecoder:
    doDecode();
    if (speedChanged) {
      speedChanged = false;
      displayCWspeed (CWsettings.wpm, true);
    }
    break;
  case morseBacklight:
      if (CWsettings.backlightOn) {
        CWsettings.backlightOn = false;
        lcd.noBacklight();
      } else {
        CWsettings.backlightOn = true;
        lcd.backlight();
      }

      saveConfig();
      topMenu();
      break;
  case morseQuickEcho:
    if(secondaryMode == SEC_MODE_GENERATE) {
      generateCW();
      if(quickEcho_wordDone) {
#define quickEcho_keyedSigs copyGame_keyedChars
        quickEcho_keyedSigs = signCounter;

        // save number of characters actually displayed. This is
        // needed for displaying the result correctly, later.
        secondaryMode = SEC_MODE_INPUT;
        clearCounters();
      }
    }else if(secondaryMode == SEC_MODE_INPUT){
      // check all signs
      boolean correct = true;
      for(byte l = 0; l < signCounter && qso_name1[l] != BLANK_IDX; l++)
        correct = correct && qso_name1[l] == qso_name2[l];

      if(!correct) {
        // there is a wrong sig
        vol.tone(sidetonePin, NOTE_A3_FREQ,
                 realVolume[CWsettings.sidetoneVolume]);
        delay(100);
        vol.noTone();
        pushChar(' ', true);
        accelerate(false);
        // repeat the word one time, if it was wrong.
        quickEchoGoAhead(!quickEcho_lastWordCorrect);
        quickEcho_lastWordCorrect = !quickEcho_lastWordCorrect;
      }else if(copyGame_keyedChars == signCounter){
        pushChar(' ', true);
        // word is complete and correct
        accelerate(true);
        // get a new word and go ahead
        quickEchoGoAhead(true);
        quickEcho_lastWordCorrect = true;
      }else if (doPaddleIambic(leftKey, rightKey))
        // we are busy keying and so need a very tight loop !
        return;
    }
  case morseCopyGame:
    if(secondaryMode == SEC_MODE_GENERATE) {
      generateCW();
      // now generate a certain amount of CW signs

      if(signCounter == COPY_GAME_N_SIGS) {

        // save number of characters actually displayed. This is
        // needed for displaying the result correctly, later.
        copyGame_generatedChars = charCounter;

        delay(500);
        // erase the textBuffer
        pushChar('\0', false);

        fullScreenMsg(F("nw btu pse cfm"));

        clear_PaddleLatches();
        //animateCountdown();

        secondaryMode = SEC_MODE_INPUT;
        clearCounters();
      }
    }else if(secondaryMode == SEC_MODE_INPUT){
      if(signCounter == COPY_GAME_N_SIGS) {
        // so, the user has entered his signs. Now compare them to
        // the generated signs and display the result.

        // save number of characters actually displayed. This is
        // needed for displaying the result correctly, later.
        copyGame_keyedChars = trimKeyedChars(charCounter,
                                             copyGame_generatedChars);
        copyGame_keyedChars_without_blanks = 0;
        for (int16_t i = textBufferIndex; textBuffer[i] != '\0';
             i = textbufModulo(i-1)) {
          copyGame_keyedChars_without_blanks += textBuffer[i] != ' ';
        }

        // fill up with blanks, if the signs we keyed correspond to
        // less characters than have been generated before.
        while(copyGame_generatedChars > copyGame_keyedChars){
          pushChar(' ', false);
          copyGame_keyedChars++;
        }

        delay(500);
        fullScreenMsg(F("r mni tnx"));

        int8_t i;
        // do not display the previously generated text, but count
        // the number of correctly keyed signs
        correctCount = compareCopyGameResult(&i, 0, false);

        pushChar(' ',false);
        // arrow pointing right
        pushChar(ARROW_RIGHT_SYMBOL, false);

        uint8_t correct_percent = (100*correctCount)/copyGame_keyedChars_without_blanks;
        sprintf(formatIntegerBuffer, "%2i", correct_percent);
        pushString(formatIntegerBuffer);
        pushChar('%', true);
        // the length of the ' >99%' snippet
#define PERCENTAGE_SNIPPET_LENGTH  5
        resultSnippetLength = PERCENTAGE_SNIPPET_LENGTH;
        if(correct_percent >= 90) {
          pushChar(' ', false);
          resultSnippetLength++;
          const char ufb[] = "ufb";
          pushString(ufb);
          resultSnippetLength += 3;
        }else if(correct_percent >= 70) {
          pushChar(' ', false);
          resultSnippetLength++;
          pushChar(SMILEY_SYMBOL2, true);
          resultSnippetLength++;
        }else if(correct_percent >= 50) {
          pushChar(' ', false);
          resultSnippetLength++;
          pushChar(SMILEY_SYMBOL1, true);
          resultSnippetLength++;
        }

        if(correct_percent >= 95) {
          accelerate(true);
          melody(NOTE_A4_FREQ,NOTE_CSHARP5_FREQ,NOTE_E5_FREQ);
        }else if(correct_percent < 50) {
          accelerate(false);
          melody(NOTE_FSHARP4_FREQ,NOTE_CSHARP4_FREQ,NOTE_A3_FREQ);
        }

        secondaryMode = SEC_MODE_PROCESS;
        showCopyGameResult(resultSnippetLength);
      }else if (doPaddleIambic(leftKey, rightKey))
        // we are busy keying and so need a very tight loop !
        return;
    }else if(secondaryMode == SEC_MODE_PROCESS){

      // just wait.

      // at paddle squeeze, start a new round.
      if (leftKey && rightKey)   {
        // for debouncing:
        // wait until paddles are released
        while (checkPaddles())
          ;
        initNewCopyGameRound();
      }
    }
    break;
  default:
    // this must never happen!
    morseState = morseKeyer;
  }



  // if we have time then check for button presses
  modeButton.Update();
  // action during decoder mode!
  if (morseState == morseDecoder && modeButton.clicks) {
    topMenu();
    return;
  }
  // actions based on encoder button
  switch (modeButton.clicks) {
  case 1:
    if(morseState == morseCopyGame){
      if(secondaryMode == SEC_MODE_PROCESS) {
        showMarks = !showMarks;
        showCopyGameResult(resultSnippetLength);
      }
    } else {
      //switch to next encoder Mode
      encoderState = (encoderMode)((encoderState + numEncoderModes + 1) % numEncoderModes);
      displayEncoderMode(true);
      enterOwnSigsForGenerator();
    }
    break;
  case 2:
    if(morseState == morseCopyGame){
      if(secondaryMode == SEC_MODE_PROCESS) {
        showMarks = !showMarks;
        showCopyGameResult(resultSnippetLength);
      }
    }
#ifdef ENABLE_MEMO_KEYER
    else if(morseState == morseKeyer){
      if(generatorMode == MEMO_KEYER_TEXT){
        generatorMode = GROUPOF5;
        generatorState = KEY_UP;
        vol.noTone();
      }else {
        // key the auto-text from the EEPROM
        generatorMode = MEMO_KEYER_TEXT;
        active = true;
        startCW();
        prepNewGeneratorRun(false);
      }
    }
#endif
    else {
      encoderState = (encoderMode)((encoderState + numEncoderModes - 1) % numEncoderModes);
      displayEncoderMode(true);
      enterOwnSigsForGenerator();
    }
    break;
  case -1:
    if (encoderState != scrollMode) {
      encoderState = scrollMode;
      if (settingsDirty) {
        // if settings have been changed:
        // save the config in EEPROM when we return to scrollMode
        saveConfig();
        settingsDirty = false;
      }
      displayEncoderMode(true);
      enterOwnSigsForGenerator();
    } else {
#ifdef ENABLE_MEMO_KEYER
      if(generatorMode == MEMO_KEYER_TEXT)
        generatorMode = GROUPOF5;
      if(morseState == morseSetAutoKeyerText && signCounter)
        EEPROM.updateByte(addressMemoText + signCounter_inclBlanks, NOT_IN_POOL_IDX);
#endif
      topMenu();
      return;
    }
    break;
  }

  // and we have time to check the encoder
  if(encoderDelta) {
    switch (encoderState) {
    case scrollMode:
      if(!active) {
        // scrolling is only allowed when the training generator is paused.
        scrollPosition += encoderDelta;
        scrollPosition = constrain(scrollPosition, 0, SCROLL_LENGTH-1);

        updateDisplayLine();
        if(morseState == morseCopyGame && secondaryMode == SEC_MODE_PROCESS){
          showCopyGameResult(resultSnippetLength);
        }
      }
      break;
    case speedSettingMode:
      CWsettings.wpm += encoderDelta;
      reCalcSpeedSetting();

      // update display of CW speed
      displayCWspeed(CWsettings.wpm, true);
      clearCounters();
      break;
    case curtisSettingMode:
      if (morseState == morseKeyer ||
#ifdef ENABLE_MEMO_KEYER
          morseState == morseSetAutoKeyerText ||
#endif
          morseState == morseDecoder)
        setCurtisMode();
      else
        setFarnsworthMode();
      break;
    case polaritySettingMode:
      if (morseState == morseKeyer ||
#ifdef ENABLE_MEMO_KEYER
          morseState == morseSetAutoKeyerText ||
#endif
          morseState == morseDecoder)
        setPolarityMode();
      else
        setGeneratorMode();
      break;
    case pitchSettingMode:
      CWsettings.sidetoneFreq += encoderDelta * 50;
      CWsettings.sidetoneFreq = constrain(CWsettings.sidetoneFreq, 250, 950);
      displayPitch();
      settingsDirty = true;
      displayEncoderMode(false);
      break;
    case volumeSettingMode:
      CWsettings.sidetoneVolume += encoderDelta;
      CWsettings.sidetoneVolume = constrain(CWsettings.sidetoneVolume, 0, 8);
      displayVolume();
      settingsDirty = true;
      displayEncoderMode(false);
      break;
    default:
      // this must never happen!
      encoderState = scrollMode;
      break;
    }
    // reset the encoder
    encoderDelta = 0;
  }
}


void setCurtisMode() {
  CWsettings.keyermode = (keyer_mode) ((CWsettings.keyermode + encoderDelta + numKeyerModes) % numKeyerModes);
  displayCurtisMode(CWsettings.keyermode);              // and display
  settingsDirty = true;
  displayEncoderMode(false);
}

void setFarnsworthMode() {
#define FARNSWORTH_MAX_FACTOR 9
  // the lowest value is 1, not 0
  CWsettings.farnsworthMode = 1 + ((CWsettings.farnsworthMode-1) +
              FARNSWORTH_MAX_FACTOR + encoderDelta) % FARNSWORTH_MAX_FACTOR;
  displayFarnsworthMode();
  interCharacterSpace = 3*ditLength*CWsettings.farnsworthMode;
  interWordSpace = morseState == morseQuickEcho ? 0 : 7*ditLength*CWsettings.farnsworthMode;

  settingsDirty = true;
  displayEncoderMode(false);
}

void setPolarityMode() {
  CWsettings.didah = !CWsettings.didah;

  displayPolarity();
  settingsDirty = true;
  displayEncoderMode(false);
}

void setGeneratorMode() {
  CWsettings.generatorMode = (CWsettings.generatorMode + encoderDelta +
               NUMBER_OF_TRAINING_GROUPS) % NUMBER_OF_TRAINING_GROUPS;
  displayGeneratorMode();
  updateGeneratorMode();
  prepNewGeneratorRun(morseState == morseTrainer);
  settingsDirty = true;

  displayEncoderMode(false);
}

void updateGeneratorMode() {
  /// set the boundaries, the mode (GROUPOF5 or CALLSIGNS or ABBREVS)
  if (CWsettings.generatorMode < 9) {
    generatorMode = GROUPOF5;
    startPool = bounds[CWsettings.generatorMode][0];
    endPool = bounds[CWsettings.generatorMode][1] + 1;
  }
  else if (CWsettings.generatorMode == 9)
    generatorMode = CALLSIGNS;
  else if (CWsettings.generatorMode == 10)
    generatorMode = ABBREVS;
  else if (CWsettings.generatorMode == 11){
    generatorMode = CHOICE;
    // empty the buffer of the sigs. this triggers asking for sigs at
    // the next squeeze.
    qso_callsign1[0] = END_OF_WORD_IDX;
  }
#ifdef ENABLE_QSOTEXT_GENERATOR
  else if (CWsettings.generatorMode == 12){
    generatorMode = QSOTEXT;
    // make sure the groupof5 generated within the qso text are just
    // letters and do not contain prosigns or punctuation.
    startPool = bounds[BOUNDS_ALPHANUMERIC][0];
    endPool = bounds[BOUNDS_ALPHANUMERIC][1] + 1;
  }
#endif
#ifdef ENABLE_TEST_GENERATOR
  else if (CWsettings.generatorMode == 13)
    generatorMode = TEST_ALL_SIGNS;
#endif

  // the MEMO_KEYER_TEXT generator is not available in the CW Trainer.

  else {
    generatorMode = KOCH;
    kochLevel = CWsettings.generatorMode - GENERATOR_MODE_KOCH_OFFSET;
  }

}

void prepNewGeneratorRun(boolean doStartSeq){
  // use the user interaction to seed the random number generator
  randomSeed(millis());

  // when the training group has been changed, begin with a start
  // sequence
  if(doStartSeq) {
    k_startSeq = 0;
    doStartSequence = true;
  }else{
    doStartSequence = false;
  }
  // and reset counters, too. For Koch method, this means the
  // generator will begin to spit out only signs of the current
  // level (not inkl older signs).
  clearCounters();
#ifdef ENABLE_QSOTEXT_GENERATOR
  if (generatorMode == QSOTEXT) initNewQSO();
#endif

  // force new word when the next sig will be fetched..
  icurrent_sig = MAX_SIG_WORD_LENGTH;
  fetchNextSig();

}

// functions related to paddle actions.  paddle reads a capacitive
// sensor, connected to pin, and returns a logical value true or
// false.  but if mdoe is set to use external apddles, we just check
// if the input has been down close to 0
boolean checkPaddles() {
  if (CWsettings.useExtPaddle) {
    // see if paddles shortened to GROUND
    pinMode(leftPin, INPUT_PULLUP);
    if (analogRead(leftPin) < 50) {
      leftKey = true;
    }
    else
      leftKey = false;
    pinMode(rightPin, INPUT_PULLUP);
    if (analogRead(rightPin) < 50) {
      rightKey = true;
    }
    else
      rightKey = false;
  } else {                                  // evaluate sensors
    value0 = ADCTouch.read(leftPin, 12);   //no second parameter
    value1 = ADCTouch.read(rightPin, 12);     //   --> 100 samples

    value0 -= refLeft;       //remove offset
    value1 -= refRight;

    if (value0 > 40) {
      // adaptive calibration
      CWsettings.tLeft = (7*CWsettings.tLeft +
                          (int)((float)value0 * SENS_FACTOR)  ) / 8;
    }
    if (value0 > CWsettings.tLeft)
      leftKey = true;
    else
      leftKey = false;

    if (value1 > 40) {// adaptive calibration
      CWsettings.tRight = (7*CWsettings.tRight +
                           (int)((float)value1 * SENS_FACTOR) ) / 8;
    }
    if (value1 > CWsettings.tRight)
      rightKey = true;
    else
      rightKey = false;
  }
  return leftKey || rightKey;
}


// Given the paddle state, generate dits and dahs, in the chosen
// iambic keying system
boolean doPaddleIambic (boolean leftKey, boolean rightKey) {
  // temp storage if we need to swap left and right
  boolean paddleSwap;
  // timer for current element (dit or dah)
  static unsigned long ktimer;
  // timer for early paddle latch in Curtis mode B+
  static unsigned long curtistimer;
  if (!CWsettings.didah)   {
    // swap left and right values if necessary
    paddleSwap = leftKey;
    leftKey = rightKey;
    rightKey = paddleSwap;
  }

  switch (keyerState) { // this is the keyer state machine
  case IDLE_STATE:
    // display the interword space, if necessary
    if (millis() > interWordTimer) {
      treeptr = CWTREE_BLANK_SIG;
      displayMorse();
      // the biggest possible unsigned long number - do not
      // output extra spaces!
      interWordTimer = ~0ul;
    }
    // Was there a paddle press?
    if (leftKey || rightKey) {
      update_PaddleLatch(leftKey, rightKey);  // trigger the paddle latches
      treeptr = CWTREE_ROOT;

      if (leftKey) {
        setDITstate();          // set next state
        DIT_FIRST = true;          // first paddle pressed after IDLE was a DIT
      }
      else {
        setDAHstate();
        DIT_FIRST = false;         // first paddle was a DAH
      }
    }
    // we return false if there was no paddle press in IDLE STATE
    // - Arduino can do other tasks for a bit
    else return false;
    break;

  case DIT:
    // first we check that we have waited as defined by ACS settings
    if (CWsettings.ACS && (millis() <= acsTimer))
      // if we do automatic character spacing, and haven't waited for 3 dits...
      break;
    // always clear the paddle latches at beginning of new element
    clear_PaddleLatches();
    // remember that we process a DIT
    keyerControl |= DIT_LAST;

    ktimer = ditLength; // prime timer for dit
    switch (CWsettings.keyermode) {
      // Curtis mode B registers paddle presses almost
      // immediately when element begins
    case KEYER_MODE_IAMBICB:  curtistimer = max(12, ditLength /6) ;
      break;
    case KEYER_MODE_IAMBICBplus:
      // enhanced Curtis mode B starts checking after 40% of a dit
      curtistimer = ditLength * .4;
      break;
    case KEYER_MODE_IAMBICA:
    case KEYER_MODE_ULTIMATIC:
      // no early paddle checking in Curtis mode A or Ultimatic mode
      curtistimer = ditLength;
      break;
    default:
      // this must never happen
      CWsettings.keyermode = KEYER_MODE_IAMBICA;
      break;
    }
    // set next state of state machine
    keyerState = KEY_START;
    break;

  case DAH:
    // first we check that we have waited as defined by ACS settings

    // if we do automatic character spacing, and haven't waited for 3 dits...
    if (CWsettings.ACS && (millis() <= acsTimer))
      break;
    clear_PaddleLatches();        // clear the paddle latches
    // clear dit latch  - we are not processing a DIT
    keyerControl &= ~(DIT_LAST);

    ktimer = dahLength;
    switch (CWsettings.keyermode) {
    case KEYER_MODE_IAMBICB:
      // Curtis mode B registers paddle presses almost immediately
      // when element begins
      curtistimer = max(12, ditLength /6) ;
      break;
    case KEYER_MODE_IAMBICBplus:
      // enhanced Curtis mode B starts checking after 40% of dah length
      curtistimer = dahLength * .4;
      break;
    case KEYER_MODE_IAMBICA:
    case KEYER_MODE_ULTIMATIC:
      // no early paddle checking in Curtis mode A or Ultimatic mode
      curtistimer = dahLength;
      break;
    default:
      // this must never happen
      CWsettings.keyermode = KEYER_MODE_IAMBICA;
      break;
    }
    // set next state of state machine
    keyerState = KEY_START;
    break;



  case KEY_START:
    // Assert key down, start timing, state shared for dit or dah

    // turn the LED on, key transmitter or whatever
    // digitalWrite(ledPin, HIGH);
    // start generating side tone
#ifdef VOLUME2
    vol.tone(CWsettings.sidetoneFreq, SINE,
             realVolume[CWsettings.sidetoneVolume]);
#else
    vol.tone(sidetonePin, CWsettings.sidetoneFreq,
             realVolume[CWsettings.sidetoneVolume]);
#endif
    // turn the LED on, key transmitter, or whatever
    digitalWrite(keyerPin, HIGH);

    ktimer += millis();                     // set ktimer to interval end time

    curtistimer += millis(); // set curtistimer to curtis end time
    keyerState = KEYED; // next state
    break;

  case KEYED:
    // Wait for timers to expire
    if (millis() > ktimer) {                // are we at end of key down ?
      // turn the LED off, unkey transmitter, or whatever
      digitalWrite(keyerPin, LOW);
      vol.noTone();                     // stop side tone
      ktimer = millis() + ditLength;    // inter-element time
      keyerState = INTER_ELEMENT;       // next state
    }
    // in Curtis mode we check paddle as soon as Curtis time is off
    else if (millis() > curtistimer ) {
      // but we remain in the same state until element time is off!
      update_PaddleLatch(leftKey, rightKey);
    }
    break;

  case INTER_ELEMENT:
    // latch paddle state while between elements
    update_PaddleLatch(leftKey, rightKey);
    // at end of INTER-ELEMENT
    if (millis() > ktimer) {
      switch(keyerControl) {
      case 3:
        // both paddles are latched
      case 7:
        if (CWsettings.keyermode <= 2)
          // we are in a Iambic mode
          if (keyerControl & DIT_LAST)
            // last element was a dit - this is case 7, really
            setDAHstate();
        // next element will be a DAH
          else
            // and this is case 3 - last element was a DAH
            setDITstate();
        // the next element is a DIT
        else
          // in Ultimatic mode
          if (DIT_FIRST)
            // when first element was a DIT
            setDAHstate();
        // next element is a DAH
          else
            // but when first element was a DAH
            setDITstate();
        // the next element is a DIT!
        break;
        // dit only is latched, regardless what was last element
      case 1:
      case 5:
        setDITstate();
        break;
        // dah only latched, regardless what was last element
      case 2:
      case 6:
        setDAHstate();
        break;
        // none latched, regardless what was last element
      case 0:
      case 4:
        // we are at the end of the character and go
        // back into IDLE STATE
        keyerState = IDLE_STATE;
        // display the decoded morse character(s)
        displayMorse();

        // if we have seen n chars since
        // changing speed, we write the config
        // to EEPROM.
        if (signCounter == N_SIGNS_THEN_SAVE_CONFIG) {
          saveConfig();
        }
        // prime the ACS timer
        acsTimer = millis() + (ACS_LENGTH * ditLength);
        // prime the timer to detect a space
        // between characters
        interWordTimer = millis() + 6*ditLength;
        // nominally 7 dit-lengths, but we are
        // not quite so strict here

        // clear all latches completely before we go to IDLE
        keyerControl = 0;

        break;
      } // switch keyerControl : evaluation of flags
    } // end of INTER_ELEMENT
  } // end switch keyerState - end of state machine
  if (keyerControl & 3) // any paddle latch?
    // we return true - we processed a paddle press
    return true;
  else
    // when paddle latches are cleared, we return false
    return false;
}


///
/// Keyer subroutines
///

// update the paddle latches in keyerControl
void update_PaddleLatch(boolean dit, boolean dah)
{
  if (dit)
    keyerControl |= DIT_L;
  if (dah)
    keyerControl |= DAH_L;
}

// clear the paddle latches in keyer control
void clear_PaddleLatches ()
{
  keyerControl &= ~(DIT_L + DAH_L);   // clear both paddle latch bits
}

// functions to set DIT and DAH keyer states
void setDITstate() {
  keyerState = DIT;
  treeptr = CWtree[treeptr].dit;
}

void setDAHstate() {
  keyerState = DAH;
  treeptr = CWtree[treeptr].dah;
}


// toggle polarity of paddles
void togglePolarity () {
  CWsettings.didah = !CWsettings.didah;
  displayPolarity();
}


/// displaying decoded morse code
void displayMorse() {
  if (treeptr == CWTREE_ROOT)
    return;
  if(generatorMode == CHOICE &&
     secondaryMode == SEC_MODE_INPUT) {
    if(CWtree[treeptr].sigidx == NOT_IN_POOL_IDX){
      // this is <ar> or max number of sigs.
      // this is the end of interactive input of sigs by the user.

      // misuse the sig buffer from qso generator:
      qso_callsign1[signCounter] = END_OF_WORD_IDX;

      // will process further in the main loop...
      secondaryMode = SEC_MODE_PROCESS;

      // reset tree pointer
      treeptr = CWTREE_ROOT;
      // do not display it in this case.
      return;
    }else if(CWtree[treeptr].sigidx != BLANK_IDX){
      // remember this sign, to use it later for generating groups
      qso_callsign1[signCounter] = CWtree[treeptr].sigidx;
      // continue below, to display it.
    }
  }else if(morseState == morseQuickEcho) {
    // misuse the sig buffer from qso generator:
    qso_name2[signCounter] = CWtree[treeptr].sigidx;
  }
#ifdef ENABLE_MEMO_KEYER
  else if(morseState == morseSetAutoKeyerText) {
    if(CWtree[treeptr].sigidx == NOT_IN_POOL_IDX || signCounter_inclBlanks == MAX_MEMO_TEXT_LENGTH-1){
      EEPROM.updateByte(addressMemoText + signCounter_inclBlanks, NOT_IN_POOL_IDX);

      fullScreenMsg(F("hv saved ur msg."));
      // reset tree pointer
      treeptr = CWTREE_ROOT;

      // leave
      topMenu();

      // cannot display it in this case.
      return;
    }else
      EEPROM.updateByte(addressMemoText + signCounter_inclBlanks, CWtree[treeptr].sigidx);

  }
#endif

  fillSigString(CWtree[treeptr].sigidx);

  // now display the string sigString ....
  pushString(sigString);
  if(CWtree[treeptr].sigidx != BLANK_IDX)
    // count this sign
    ++signCounter;
  signCounter_inclBlanks++;
  // count also the characters actually displayed
  charCounter += strlen(sigString);

  // reset tree pointer
  treeptr = CWTREE_ROOT;

  if(generatorMode == CHOICE &&
     secondaryMode == SEC_MODE_INPUT &&
     signCounter == MAX_SIG_WORD_LENGTH-1) {
    // the buffer is full, cannot remember further sigs.
    qso_callsign1[MAX_SIG_WORD_LENGTH] = END_OF_WORD_IDX;
    // will process further in the main loop...
    secondaryMode = SEC_MODE_PROCESS;
  }
}

////////////////////////////////////////////////////////
// display functions for a scrolling display in 2nd line

void updateDisplayLine () {
  // Show a piece of the text buffer.
  // write a line of text into the second row of the LCD
  /* if (textBufferIndex < 0) */
  /*   // nothing in the buffer */
  /*   return; */

  char c;
  boolean printAnything = true;
  boolean isEmpty = false;
  // check, if there was a \0 already *before* the character that will
  // go into the rightmost lcd cell
  int16_t i;
  for(i = scrollPosition; i>0; i--) {
    if(textBuffer[textbufModulo(textBufferIndex - scrollPosition + i)] == '\0') {
      printAnything = false;
      isEmpty = i == scrollPosition;
      break;
    }
  }

  if (printAnything) {
    // fetch chars from the queue as long as they are not \0, , wrap
    // around to access the buffer in a circular way
    i = 0;
    while(i < LCD_COLUMNS
          && (c = textBuffer[textbufModulo(textBufferIndex - scrollPosition - i)])) {
      // align the text on the right side
      lcd.setCursor(LCD_COLUMNS - 1 - i, 1);
      lcd.print(c);
      // iterate down the queue of chars
      i++;
    }
  }
  while (i < LCD_COLUMNS) {
    // fill the rest of the line with blanks.  This simplifies
    // things, because it is not necessary to contrain the
    // scrollPosition to the number of chars which have been pushed so far.
    // We can also very simply clear the display line by pushing a '\0'.
    lcd.setCursor(LCD_COLUMNS - 1 - i, 1);

#define ARROW_POSITION 2
    // dont draw the arrow on top of the blanks, to avoid flickering
    if (i == ARROW_POSITION && !printAnything && !isEmpty) {
      // show a little arrow, to indicate that there is something to
      // display if the user scrolls
      lcd.print((char)126);
    } else {
      lcd.print(' ');
    }
    i++;
  }

}


void pushChar (char c, boolean updateDisplay) {
  // push one character to the circular buffer

  // make sure that the display shows the end of the queue when we key
  // new characters
  scrollPosition = 0;

  textBufferIndex = textbufModulo(textBufferIndex + 1);
  textBuffer[textBufferIndex] = c;
  textBuffer[textbufModulo(textBufferIndex+1)] = '\0';

  // and display the line
  if (updateDisplay)
    updateDisplayLine();

#ifdef ENABLE_USB_KEYBOARD_OUTPUT
  // translate the lcd device symbol into ascii.
  switch(c) {
  case '\0':
    return;
  case AE_SYMB: c = 132;
    break;
  case OE_SYMB: c = 148;
    break;
  case UE_SYMB: c = 129;
    break;
  case UNKNOWN_SIG_SYMB:
    // black block
    /* c = (char) 219; */
    /* // gray background */
    /* c = (char) 177; */
    c = '_';
    break;
  case SMILEY_SYMBOL1:
  case SMILEY_SYMBOL2:
  case ARROW_RIGHT_SYMBOL:
    return;
  }
  Keyboard.print(c);
#endif
}

void pushString (const char str[]) {

  char c;
  while ((c = *(str++))) {
    pushChar(c, false);
#ifdef USB_KEYBOARD_OUTPUT
    Keyboard.write(c);
#endif
  }

  updateDisplayLine();

}


void clearTopLine (boolean arrowsRight) {
  // for config settings, clear line and display little arrows, unless
  // in decoder mode
  if (morseState != morseDecoder) {
    lcd.setCursor(0, 0);
    // put blanks in top line
    for (uint8_t i = 0; i < LCD_COLUMNS; i++)
      lcd.print(' ');
    if (arrowsRight)
      //arrows on the right
      lcd.setCursor(LCD_COLUMNS - 2 - 1, 0);
    else
      //arrows on the left
      lcd.setCursor(0, 0);

    lcd.print(ARROW_LEFT_SYMBOL);
    lcd.print(ARROW_RIGHT_SYMBOL);
  }
}

void displayTopLine() {
  // update display of CW speed

  displayCWspeed(CWsettings.wpm,
                 morseState == morseKeyer ||
#ifdef ENABLE_MEMO_KEYER
                 morseState == morseSetAutoKeyerText ||
#endif
                 morseState == morseDecoder);
  if (morseState == morseKeyer ||
#ifdef ENABLE_MEMO_KEYER
      morseState == morseSetAutoKeyerText ||
#endif
      morseState == morseDecoder) {
    displayCurtisMode(CWsettings.keyermode);              // and of Curtis mode
    displayPolarity();                                    // and paddle polarity
  } else {
    displayFarnsworthMode();
    displayGeneratorMode();
  }

  displayPitch();                                       // sidetone frequency
  displayVolume();                                      // sidetone volume
}

void displayCWspeed (int wpm, boolean withUnitString) {
  // displayCurtisMode(CWsettings.keyermode);            // display keyer mode
  lcd.setCursor(lcdSpeed, 0);      // and current speed
  sprintf(formatIntegerBuffer, "%2i", wpm);
  lcd.print(formatIntegerBuffer);
  if (withUnitString)
    lcd.print(F("wpm"));
  else
    lcd.print(F("   "));
}

void displayCurtisMode(byte mode) {
  // this is what we display in Curtis setting Mode
  lcd.setCursor(lcdMode, 0);
  lcd.print(keyer_modes[mode]);
}

void displayPolarity () {
  lcd.setCursor(lcdPolarity, 0);
  lcd.print(CWsettings.didah ? "._" : "_.");
}

void displayPitch () {
  lcd.setCursor(lcdPitch, 0);
  lcd.print (CWsettings.sidetoneFreq);
}

void displayVolume () {
  lcd.setCursor(lcdVolume, 0);
  if (CWsettings.sidetoneVolume)
    // for a nonzero volume, display a little symbol
    lcd.write(byte(CWsettings.sidetoneVolume - 1));
  else
    lcd.print(' ');
}

void displayFarnsworthMode() {
  lcd.setCursor(lcdFarnsworth, 0);
  lcd.print('*');
  lcd.print(CWsettings.farnsworthMode);
}

void displayGeneratorMode() {
  lcd.setCursor(lcdSymbols, 0);
  if(CWsettings.generatorMode < GENERATOR_MODE_KOCH_OFFSET) {
    char c;
    const char* ptr = (const char*) pgm_read_word(&groups[CWsettings.generatorMode]);
    if(ptr)
      while(c = pgm_read_byte(ptr++))
        lcd.print(c);
  } else {
    lcd.print(F("Koch"));
    sprintf(formatIntegerBuffer, "%02i",
            CWsettings.generatorMode-GENERATOR_MODE_KOCH_OFFSET+1);
    lcd.print(formatIntegerBuffer);
  }

}

void displayEncoderMode(boolean enteringMode) {
  if (enteringMode)
    clearTopLine(encoderState == speedSettingMode);

  switch (encoderState) {
    case scrollMode:
      if (enteringMode)
        displayTopLine();
      break;
    case speedSettingMode:
      displayCWspeed (CWsettings.wpm, true);
      break;
    case curtisSettingMode:
      if (morseState == morseKeyer ||
#ifdef ENABLE_MEMO_KEYER
          morseState == morseSetAutoKeyerText ||
#endif
          morseState == morseDecoder)
        displayCurtisMode(CWsettings.keyermode);
      else
        displayFarnsworthMode();
      break;
    case polaritySettingMode:
      if (morseState == morseKeyer ||
#ifdef ENABLE_MEMO_KEYER
          morseState == morseSetAutoKeyerText ||
#endif
          morseState == morseDecoder)
        displayPolarity();
      else
        displayGeneratorMode();
      break;
    case pitchSettingMode:
      displayPitch();
      break;
    case volumeSettingMode:
      displayVolume();
      break;
  default:
    //this must never happen!
    encoderState = scrollMode;
    break;
  }
}


/// functions for training mode

void enterOwnSigsForGenerator(){
  if(morseState == morseTrainer &&
     encoderState == scrollMode &&
     generatorMode == CHOICE) {
    clearAndPrintScreen(F("qrv pse k = qru"),
                        F("wid unknown sig"));

    clearCounters();

    secondaryMode = SEC_MODE_INPUT;

    // do not output a blank in the next few millisecs, interpreting
    // the silence as a blank.  This is not desired because it
    // immediately overwrites the lower line of the screen.
    interWordTimer = ~0ul;

  }
}

void startCW() {
  generatorState = KEY_UP;
}

void generateCW () {
  // we use a list of elements for that character, and the number of elements
  if (millis() > timer) {                // are we at end of key down ?

    switch (generatorState) {           // CW generator state machine
    case KEY_DOWN:
      vol.noTone();                     // stop side tone
      digitalWrite(keyerPin, LOW);      // if we key in trainer mode, or not....
      ++pointer;
      if (pointer < sig_number_of_cw_elems) {
        // we need to output another element for this character

        // pause is inter element (= 1 dit = ditLength)
        timer = millis() + ditLength;
        generatorState = KEY_UP;     // next state
      } else {
        // display the old character before we begin with a new one
        if(morseState != morseQuickEcho)
          pushString(sigString);
        if(sigString[0] != ' ')
          ++signCounter;
        signCounter_inclBlanks++;
        charCounter += strlen(sigString);

        // retrieve the next sign we should output
        fetchNextSig();
        // reset pointer to start of list of elements
        pointer = 0;

        // next space is between characters or between words?
        timer = millis() + (sig_number_of_cw_elems == 0 ? interWordSpace : interCharacterSpace);

        // next state
        generatorState = KEY_UP;
      }

      break;
    case KEY_UP:
      // start tone for next element, if there is any

      if (sig_number_of_cw_elems != 0) {
        // start generating side tone
        vol.tone(sidetonePin, CWsettings.sidetoneFreq,
                 realVolume[CWsettings.sidetoneVolume] );
        if (keyTrainerMode)
          // turn the LED on, key transmitter, or whatever
          digitalWrite(keyerPin, HIGH);
        // start a dit or a dah and set timer accordingly
        timer = millis() + (sig_cw_elems[pointer] ? dahLength : ditLength);
      }
      generatorState = KEY_DOWN;         // next state
      break;
    }
  }
}

void fetchNextWord() {
  if (doStartSequence) {
    byte koch_signs_in_this_level = min(KOCH_SIGNS_PER_LEVEL,
                      KOCH_ORDER_SIZE - kochLevel * KOCH_SIGNS_PER_LEVEL);

    if (k_startSeq == 0) {
      memcpy(current_sig_word, startSequence, START_SEQ_LENGTH);
      current_sig_word[START_SEQ_LENGTH] = END_OF_WORD_IDX;
      ++k_startSeq;
    }else if (generatorMode == KOCH &&
              k_startSeq >= 1 &&
              k_startSeq <= koch_signs_in_this_level * KOCH_START_SEQ_N) {
  // letter spacing when introducing the new signs of the Koch level
#define KOCH_START_SEQ_INTERVAL 7
  // put the letter right in the middle of the interval:
#define DELTA ((KOCH_START_SEQ_INTERVAL-1)/2)

      for(uint8_t i = 0; i < KOCH_START_SEQ_INTERVAL; i++)
        current_sig_word[i] = BLANK_IDX;
      current_sig_word[DELTA] = kochOrder[kochLevel*KOCH_SIGNS_PER_LEVEL+(k_startSeq-1)/KOCH_START_SEQ_N];
      current_sig_word[KOCH_START_SEQ_INTERVAL] = END_OF_WORD_IDX;
      ++k_startSeq;
    }else{
      // we have worked the start sequence completely
      doStartSequence = false;

      // reset the counter. We do not want to count the start sequence
      clearCounters();
    }
  }

  // cannot use else here, since the flow will go into both
  // conditionals successively at the end of the start sequence.
  if (!doStartSequence) {
    // As soon as the start sequence is completed, continue with the
    // regular training
    switch (generatorMode) {
    case GROUPOF5:
      generateGroupOf5();
      break;
    case CALLSIGNS:
      generateCallsign();
      break;
    case ABBREVS:
      generateAbbrev();
      break;
#ifdef ENABLE_QSOTEXT_GENERATOR
    case QSOTEXT:
      generateQSOText();
#ifdef SIMULATE_MISTAKES
      makeMistakeAndHH();
#endif
      break;
#endif
    case CHOICE:
      generateOwnSigsGroupOf5();
      break;
    case KOCH:
      generateKoch(kochLevel);
      break;
#ifdef ENABLE_TEST_GENERATOR
    case TEST_ALL_SIGNS:
      generateTestAllSigns();
      break;
#endif
#ifdef ENABLE_MEMO_KEYER
    case MEMO_KEYER_TEXT:
      generateAutoKeyerText();
      break;
#endif
    }
  }

}

#ifdef ENABLE_QSOTEXT_GENERATOR
#ifdef SIMULATE_MISTAKES
void makeMistakeAndHH() {
  if(whichQSOElem == thisNoMistake) {
    whichQSOElem = next;
  }else if(whichQSOElem == nextAndThisNoMistake) {
    whichQSOElem = thisNoMistake;
#define EVERY_NTH_WORD_A_MISTAKE 25
  }else if(whichQSOElem == next && !random(0,EVERY_NTH_WORD_A_MISTAKE)){
    // determine the word length
    uint8_t l;
    for(l = 0; current_sig_word[l] != BLANK_IDX && l < MAX_SIG_WORD_LENGTH-4; l++);
    if(l < 2) return;
    // put a mistake
    uint8_t mistake_i = random(1, l);
    current_sig_word[mistake_i] = random(bounds[BOUNDS_ALPHANUMERIC][0], bounds[BOUNDS_ALPHANUMERIC][1]);
    current_sig_word[mistake_i+1] = HH_IDX;
    current_sig_word[mistake_i+2] = BLANK_IDX;
    current_sig_word[mistake_i+3] = END_OF_WORD_IDX;
    whichQSOElem = last;
  }
}
#endif
#endif

void fetchNextSig() {
  icurrent_sig++;
  if(icurrent_sig < MAX_SIG_WORD_LENGTH){
    if(current_sig_word[icurrent_sig] == END_OF_WORD_IDX){
      if(morseState == morseQuickEcho) {
        // put the word into a buffer for comparing later. just note
        // that we are done, for the quickEcho game.
        quickEcho_wordDone = true;
        memcpy(qso_name1, current_sig_word, ABBREV_MAX_SIZE+1);

        // we will fetch the new word only later, in quick echo game
        icurrent_sig = 0;
      }else{
        icurrent_sig = 0;
        fetchNextWord();
      }
    }
  }else{
    icurrent_sig = 0;
    fetchNextWord();
  }

  // the sig is now selected via icurrent_sig.
  // now fill the sigString and the cw elements.

  fillSigString(current_sig_word[icurrent_sig]);
  fillCWElements(current_sig_word[icurrent_sig]);

}

void fillCWElements(byte sigidx){
  if (sigidx >= BLANK_IDX)
    // space
    sig_number_of_cw_elems = 0;
  else {
    // set up sig_number_of_cw_elems (number
    // of elements) and sig_cw_elems[] (list of elements)

    // look up the corresponding morse sign in our table,
    // read it from Progmem into buffer first.
    sig_number_of_cw_elems = pgm_read_byte( &pool[sigidx][1]);
    byte bitmask = pgm_read_byte( &pool[sigidx][0]);
    for (uint8_t i=0; i<sig_number_of_cw_elems; ++i) {
      // get MSB and store it in array
      sig_cw_elems[i] = (bitmask & B10000000 ? 1 : 0 );
      // shift bitmask 1 bit to the left
      bitmask = bitmask << 1;

    }
  }
}

void fillSigString(byte sigidx){

  // given the sigidx (i.e. the index wrt the pool array), now look up
  // the corresponding string for output on
  // the display.
  // For sigs consisting of a single character this is the inverse
  // to the A2IDX() macro.

  // this terminates the string for all single characters
  sigString[1] = 0;

  if(sigidx >= bounds[BOUNDS_LETTER][0] && sigidx <= bounds[BOUNDS_LETTER][1])
    sigString[0] = sigidx + 93;
  else if (sigidx >= bounds[BOUNDS_NUMBER][0] && sigidx <= bounds[BOUNDS_NUMBER][1])
    sigString[0] = sigidx + 18;
  else
    switch(sigidx) {
    case CH_IDX:
      strcpy(sigString, "ch"); break;
    case AS_IDX:
      strcpy(sigString, "<as>"); break;
    case KA_IDX:
      strcpy(sigString, "<ka>"); break;
    case KN_IDX:
      strcpy(sigString, "<kn>"); break;
    case SK_IDX:
      strcpy(sigString, "<sk>"); break;
    case BK_IDX:
      strcpy(sigString, "<bk>"); break;
    case HH_IDX:
      strcpy(sigString, "<hh>"); break;
    case VE_IDX:
      strcpy(sigString, "<ve>"); break;
    case KK_IDX:
      strcpy(sigString, "<kk>"); break;
#define AE_SYMB 225
    case AE_IDX: sigString[0] = AE_SYMB; // ä
      break;
#define OE_SYMB 239
    case OE_IDX: sigString[0] = OE_SYMB; // ö
      break;
#define UE_SYMB 245
    case UE_IDX: sigString[0] = UE_SYMB; // ü
      break;
    case POINT_IDX: sigString[0] = '.';//sigidx +6;
      break;
    case SLASH_IDX: sigString[0] = '/';//sigidx +3;
      break;
    case COMMA_IDX: sigString[0] = ',';//sigidx +3;
      break;
    case BT_IDX: sigString[0] = '=';//sigidx +16;
      break;
    case COLON_IDX: sigString[0] = ':';//sigidx +16;
      break;
    case MINUS_IDX: sigString[0] = '-';//sigidx +2;
      break;
    case QUESTION_IDX: sigString[0] = '?';//sigidx +17;
      break;
    case AT_IDX: sigString[0] = '@';//sigidx +17;
      break;
    case AR_IDX: sigString[0] = '+';//sigidx -5;
      break;
    case BLANK_IDX: sigString[0] = ' ';
      break;
    case NOT_IN_POOL_IDX: sigString[0] = UNKNOWN_SIG_SYMB;
      break;
    }
}

void generateGroupOf5() {
  uint8_t i;
  for(i = 0; i < getGroupOf5Length(); i++) {
    current_sig_word[i] = random(startPool, endPool);
  }
  current_sig_word[i] = BLANK_IDX;
  current_sig_word[i+1] = END_OF_WORD_IDX;
}

void generateOwnSigsGroupOf5() {
  // determine the number of sigs
  byte n;
  for (n = 0; qso_callsign1[n] != END_OF_WORD_IDX || n == MAX_SIG_WORD_LENGTH; n++);

  if(n > 0) {
    // generate a group of 5 sigs from the sigs interactively chosen
    // by the user
    uint8_t i;
    for(i = 0; i< getGroupOf5Length(); i++) {
      current_sig_word[i] = qso_callsign1[random(n)];
    }
    current_sig_word[i] = BLANK_IDX;
    current_sig_word[i+1] = END_OF_WORD_IDX;
  }else{
    current_sig_word[0] = BLANK_IDX;
    current_sig_word[1] = END_OF_WORD_IDX;
  }
}

#ifdef ENABLE_MEMO_KEYER
void generateAutoKeyerText() {
  int8_t i = -1;
  do{
    ++i;
    current_sig_word[i] = EEPROM.readByte(addressMemoText + signCounter_inclBlanks + i+1);
  }while(current_sig_word[i] != NOT_IN_POOL_IDX && i < MAX_SIG_WORD_LENGTH-1);
  current_sig_word[i] = END_OF_WORD_IDX;
  if(i == 0) {
    // just something else than MEMO_KEYER_TEXT. This makes the
    // morserino responsible for input again.
    generatorMode = GROUPOF5;
  }
}
#endif

#ifdef ENABLE_QSOTEXT_GENERATOR
void generateQSOText() {
  if(!nextQSOElem(true)) {
    //before starting a new qso, generate a number of blanks to have
    //some silence.
    initNewQSO();
    uint8_t i;
    for(i = 0; i < 8; i++) {
      current_sig_word[i] = BLANK_IDX;
    }
    current_sig_word[i] = END_OF_WORD_IDX;
  }
}

void evalQSOElem(int16_t iqso) {
  char abbrev[ABBREV_MAX_SIZE];
  byte i = 0;

  // copy the abbrev into the buffer here
  byte iqso_byte = pgm_read_byte_near(&(qsotree[iqso]));
  byte currentlist_start_byte = pgm_read_byte_near(&(qsotree[getListStart(iqso)]));
  byte length;
  if(isEscSpecial(currentlist_start_byte)){
    switch(iqso_byte){
    case PAUSE_MAGIC:
      // a little bit of silence.
      length = random(2,5);
      for(i = 0; i < length; i++)
        current_sig_word[i] = BLANK_IDX;
#ifdef SIMULATE_MISTAKES
      whichQSOElem = nextAndThisNoMistake;
#endif
      break;
    case CALL1_MAGIC:
      memcpy(current_sig_word, qso_callsign1, MAX_SIG_WORD_LENGTH);
      return;
    case CALL2_MAGIC:
      memcpy(current_sig_word, qso_callsign2, MAX_SIG_WORD_LENGTH);
      return;
    case NAME1_MAGIC:
      memcpy(current_sig_word, qso_name1, 7);
      return;
    case NAME2_MAGIC:
      memcpy(current_sig_word, qso_name2, 7);
      return;
    case RANDCALL_MAGIC:
      generateCallsign();
#ifdef SIMULATE_MISTAKES
      whichQSOElem = nextAndThisNoMistake;
#endif
      return;
    case RST_MAGIC:
      //readability between 1 and 5
      current_sig_word[i++] = random(bounds[BOUNDS_NUMBER][0]+1,bounds[BOUNDS_NUMBER][0]+5+1);
      //strength between 1 and 9
      //tone between 1 and 9, or a n (for 9). usually always a 9.
      switch(random(0,3)){
      case 0:
        current_sig_word[i++] = random(bounds[BOUNDS_NUMBER][0]+1,bounds[BOUNDS_NUMBER][0]+9+1);
        current_sig_word[i++] = random(bounds[BOUNDS_NUMBER][0]+1,bounds[BOUNDS_NUMBER][0]+9+1);
        break;
      case 1:
        current_sig_word[i++] = bounds[BOUNDS_NUMBER][0]+9;
        current_sig_word[i++] = bounds[BOUNDS_NUMBER][0]+9;
        break;
      case 2:
        current_sig_word[i++] = A2IDX('n');
        current_sig_word[i++] = A2IDX('n');
        break;
      }
#ifdef SIMULATE_MISTAKES
      whichQSOElem = nextAndThisNoMistake;
#endif
      break;
    case SOTA_MAGIC:
      current_sig_word[i++] = random(bounds[BOUNDS_LETTER][0], bounds[BOUNDS_LETTER][1]+1);
      current_sig_word[i++] = random(bounds[BOUNDS_LETTER][0], bounds[BOUNDS_LETTER][1]+1);
      current_sig_word[i++] = random(bounds[BOUNDS_NUMBER][0],bounds[BOUNDS_NUMBER][1]+1);
      current_sig_word[i++] = SLASH_IDX;
      current_sig_word[i++] = random(bounds[BOUNDS_LETTER][0], bounds[BOUNDS_LETTER][1]+1);
      current_sig_word[i++] = random(bounds[BOUNDS_LETTER][0], bounds[BOUNDS_LETTER][1]+1);
      current_sig_word[i++] = BT_IDX;
#ifdef SIMULATE_MISTAKES
      whichQSOElem = nextAndThisNoMistake;
#endif
      // and directly append a number with 3 letters:
    case NUM3_MAGIC:
      current_sig_word[i++] = random(bounds[BOUNDS_NUMBER][0]+1,bounds[BOUNDS_NUMBER][1]+1);
      current_sig_word[i++] = random(bounds[BOUNDS_NUMBER][0],bounds[BOUNDS_NUMBER][1]+1);
      current_sig_word[i++] = random(bounds[BOUNDS_NUMBER][0],bounds[BOUNDS_NUMBER][1]+1);
#ifdef SIMULATE_MISTAKES
      whichQSOElem = nextAndThisNoMistake;
#endif
      break;
    case NUM2_MAGIC:
      current_sig_word[i++] = random(bounds[BOUNDS_NUMBER][0]+1,bounds[BOUNDS_NUMBER][1]+1);
      current_sig_word[i++] = random(bounds[BOUNDS_NUMBER][0],bounds[BOUNDS_NUMBER][1]+1);
#ifdef SIMULATE_MISTAKES
      whichQSOElem = nextAndThisNoMistake;
#endif
      break;
    case NUM1_MAGIC:
      current_sig_word[i++] = random(bounds[BOUNDS_NUMBER][0]+1,bounds[BOUNDS_NUMBER][1]+1);
#ifdef SIMULATE_MISTAKES
      whichQSOElem = nextAndThisNoMistake;
#endif
      break;
    case REPEAT_MAGIC:
      // just repeat the last word once again.
      // NOTE: you cannot use this to repeat a list!
      // only the word currently in the buffer will be repeated.
#ifdef SIMULATE_MISTAKES
      whichQSOElem = nextAndThisNoMistake;
#endif
      return;
    case GROUPOF5_MAGIC:
      startPool = bounds[BOUNDS_ALPHANUMERIC][0];
      endPool = bounds[BOUNDS_ALPHANUMERIC][1] + 1;
      generateGroupOf5();
#ifdef SIMULATE_MISTAKES
      whichQSOElem = nextAndThisNoMistake;
#endif
      return;
    case LOCATOR_MAGIC:
      // two letters
      current_sig_word[i++] = random(bounds[BOUNDS_LETTER][0], bounds[BOUNDS_LETTER][1]+1);
      current_sig_word[i++] = random(bounds[BOUNDS_LETTER][0], bounds[BOUNDS_LETTER][1]+1);
      //two numbers
      current_sig_word[i++] = random(bounds[BOUNDS_NUMBER][0], bounds[BOUNDS_NUMBER][1]+1);
      current_sig_word[i++] = random(bounds[BOUNDS_NUMBER][0], bounds[BOUNDS_NUMBER][1]+1);
      // two letters
      current_sig_word[i++] = random(bounds[BOUNDS_LETTER][0], bounds[BOUNDS_LETTER][1]+1);
      current_sig_word[i++] = random(bounds[BOUNDS_LETTER][0], bounds[BOUNDS_LETTER][1]+1);
#ifdef SIMULATE_MISTAKES
      whichQSOElem = nextAndThisNoMistake;
#endif
      break;
    case DOK_MAGIC:
      // one letters
      current_sig_word[i++] = random(bounds[BOUNDS_LETTER][0], bounds[BOUNDS_LETTER][1]+1);
      //two numbers
      current_sig_word[i++] = random(bounds[BOUNDS_NUMBER][0], bounds[BOUNDS_NUMBER][1]+1);
      current_sig_word[i++] = random(bounds[BOUNDS_NUMBER][0], bounds[BOUNDS_NUMBER][1]+1);
#ifdef SIMULATE_MISTAKES
      whichQSOElem = nextAndThisNoMistake;
#endif
      break;
    default:
      // interpret the byte as a sigidx.
      current_sig_word[i++] = iqso_byte;
      break;
    }
  }else{
    strcpy_P(abbrev, (char*)pgm_read_word(&(abbreviations[iqso_byte])));
    // get the sigidx for every char and put that into the sigidx word
    // buffer.
    for(i = 0; abbrev[i] != '\0'; i++) {
      switch(abbrev[i]){
      case '?':
        current_sig_word[i] = QUESTION_IDX;
        break;
      default:
        current_sig_word[i] = A2IDX(abbrev[i]);
      }
    }
  }
  current_sig_word[i++] = BLANK_IDX;
  current_sig_word[i] = END_OF_WORD_IDX;
}
#endif

void generateCallsign() {
  // generate a new random string that looks like a callsign


  // length of callsign (including terminating BLANK_IDX / space)
  byte l = 0;
  boolean haveHostPrefix = false;

  while(true){
    // what type of prefix?
    switch (random(0,6)) {
      // make the prefixes with two letters more probable
    case 0:
    case 1:
    case 2:
      current_sig_word[l++] = random(4,30);
    case 3:
      // just one letter
      current_sig_word[l++] = random(4,30);
      break;
    case 4:
      // one letter, one number
      current_sig_word[l++] = random(4,30);
      current_sig_word[l++] = random(30,40);
      break;
    case 5:
      // one number, one letter
      current_sig_word[l++] = random(30,40);
      current_sig_word[l++] = random(4,30);
      break;
    }
    if(!haveHostPrefix && !random(0,10)){
      // this was just the pre-prefix of a ham in a foreign country
      current_sig_word[l++] = SLASH_IDX;
      haveHostPrefix = true;
    }else
      break;
  }
  // we have a prefix by now.

  // generate a number
  current_sig_word[l++] = random(30, 40);

  // generate a suffix, 1 2 or 3 chars long - we re-use prefix for
  // the suffix length
  byte prefix = random(1,4);
  // increase the likelihood for suffixes of length 2
  prefix = (prefix == 2 ? prefix :  random(1,4));
  while (prefix--) {
    current_sig_word[l++] = random(4, 30);
  } // now we have the suffix

  // are we /p or /m? - we do this only in rare cases - 1 out of 10
  if (! random(0, 10)) {
    current_sig_word[l++] = SLASH_IDX;
    switch(random(0, 5)) {
    case 0:
      current_sig_word[l++] = M_IDX;
    case 1:
      current_sig_word[l++] = M_IDX;
      break;
    case 2:
      current_sig_word[l++] = A_IDX;
      current_sig_word[l++] = M_IDX;
      break;
    case 3:
      current_sig_word[l++] = P_IDX;
      break;
    case 4:
      current_sig_word[l++] = A2IDX('q');
      current_sig_word[l++] = A2IDX('r');
      current_sig_word[l++] = A2IDX('p');
      break;
    }
  }
  // we have a complete call sign! finish by adding a placeholder for space
  current_sig_word[l++] = BLANK_IDX;
  current_sig_word[l] = END_OF_WORD_IDX;
}

void generateKoch(byte level) {
  /*

    lowest Koch level is 0, this is displayed as "Koch01".

    Generate a character according to Koch's method and the specified
    level. The higher the level, the more different letters are considered.
    New letters introduced the the given level occur more likely.

  */

  // reduce the amount of the new signs to a minimum 1/n of the
  // letters (excl. blanks) in the last phase, at higher levels
#define KOCH_MAX_PHASE 3

  // at the beginning of the level, show only new signs.
  // Mix in more and more already known signs later.
  // The amount of known signs rises with each phase.
  // Finally, this is the ratio of new signs in the character stream:
#define KOCH_PHASE_LENGTH 40
  byte phase = min(signCounter / KOCH_PHASE_LENGTH, KOCH_MAX_PHASE);

  // on average, every nth character will be a blank
#define KOCH_SPACING_RATIO 3

  byte length = morseState == morseQuickEcho ? 3 : random(3,7);
  byte rand;
  for(uint8_t i = 0; i < length; i++){
    rand = random(0, min(level, KOCH_MAX_PHASE + 1));
    if (rand < phase)
      //phase 0 -> no old signs
      //phase 1 -> 1/4 of signs are old signs
      //phase 2 -> 2/4 of signs are old signs
      //phase 3 -> 3/4 of signs are old signs
      // put a sign from the preceeding level
      // or any one of the lower levels
      current_sig_word[i] = kochOrder[random(((rand-1) || !level)?0:(level-1),
                    min(KOCH_ORDER_SIZE,max(level,1)*KOCH_SIGNS_PER_LEVEL))];
    else
      // put a sign from the current level
      current_sig_word[i] = kochOrder[random(level*KOCH_SIGNS_PER_LEVEL,
                    min(KOCH_ORDER_SIZE,(level+1)*KOCH_SIGNS_PER_LEVEL))];

  }
  current_sig_word[length++] = BLANK_IDX;
  current_sig_word[length] = END_OF_WORD_IDX;
}

#ifdef ENABLE_TEST_GENERATOR
void generateTestAllSigns() {
  // TEST_ALL_SIGNS is for debugging puposes only

  static byte sigidx = 0;
#define TEST_GENERATOR_GROUPLENGTH 5
  for(uint8_t i = 0; i < TEST_GENERATOR_GROUPLENGTH; i++){
    if (sigidx == NUMBER_OF_SIGNS_IN_POOL)
      sigidx = 0;
    else {
      sigidx++;
    }
    current_sig_word[i] = sigidx;
  }
  current_sig_word[TEST_GENERATOR_GROUPLENGTH] = END_OF_WORD_IDX;
}
#endif

void generateAbbrev() {
  // randomly fetch q-groups and abbreviations from PROGMEM and return
  // the characters byte by byte
  char abbrev[ABBREV_MAX_SIZE];

    /* Using the string table in program memory requires the use of
       special functions to retrieve the data.  The strcpy_P function
       copies a string from program space to a string in RAM
       ("buffer").  Make sure your receiving string in RAM is large
       enough to hold whatever you are retrieving from program
       space. */
    strcpy_P(abbrev, (char*)pgm_read_word(&(abbreviations[random(0, NUMBER_OF_ABBREVS)])));
    byte i;
    for(i = 0; abbrev[i] != '\0'; i++) {
      current_sig_word[i] = A2IDX(abbrev[i]);
    }
    current_sig_word[i] = BLANK_IDX;
    current_sig_word[++i] = END_OF_WORD_IDX;
}

void topMenu() {
  // display top-menu and wait for selection

  // stop side tone  - just in case
  vol.noTone();
  // turn the LED off, unkey transmitter, or whatever; just in case...
  digitalWrite(keyerPin, LOW);

  lcd.clear();
  printTopMenu(morseState);
  while (true) {
    // we leave the topMenu as soon as the button has been pressed

    modeButton.Update();
    if (modeButton.clicks) {
      if (modeButton.clicks == 2) {
        // a double click in top menu means we toggle keyer output on/off
        keyTrainerMode = !keyTrainerMode;
        clearAndPrintScreen(F("Key CW Trainer:"),
                            keyTrainerMode ? F("On") : F("Off"));
        delay(1500);
        lcd.clear();
        printTopMenu(morseState);
        modeButton.Update();
      }
      else if (modeButton.clicks == 3) {
        // a triple click in top menu means we toggle ACS on/off
        CWsettings.ACS = !CWsettings.ACS;
        clearAndPrintScreen(F("Auto Char Space:"),
                            CWsettings.ACS ? F("On") : F("Off"));
        saveConfig();
        delay(1500);
        lcd.clear();
        printTopMenu(morseState);
        modeButton.Update();
      }
      else if (modeButton.clicks == 1) {
        // a single click in top menu means enter a mode

        switch (morseState) {
#ifdef ENABLE_MEMO_KEYER
        case morseSetAutoKeyerText:
          clearCounters();
          setupKeyerMode(F("end wid unknown"));
          break;
#endif
        case morseKeyer:    setupKeyerMode(F("Start CW Keyer"));
          break;
        case morseTrainer:  setupTrainerMode();
          break;
        case morseDecoder:  setupDecoderMode();
          break;
        case morseQuickEcho:
          quickEcho_wordDone = false;
          // do not output a blank in the next few millisecs, interpreting
          // the silence as a blank.  This is not desired because it
          // immediately overwrites the lower line of the screen.
          interWordTimer = ~0ul;
          quickEcho_lastWordCorrect = true;
          if(generatorMode == CALLSIGNS
#ifdef ENABLE_QSOTEXT_GENERATOR
             || generatorMode == QSOTEXT
#endif
             ){
            // these two dont make much sense for this.
            //generatorMode = ABBREVS;
            generatorMode = GROUPOF5;
            startPool = bounds[BOUNDS_ALPHANUMERIC][0];
            endPool = bounds[BOUNDS_ALPHANUMERIC][1] + 1;
          }

        case morseCopyGame:
          setupCopyGameMode();
          break;
        default:
          // this catches the others, just to make the compiler silent
          break;
        }

        // we got a click, so leave the Menu
        return;
      }
    }

    if (encoderDelta) {
      morseState = (morserinoMode)((morseState + numMorserinoModes + encoderDelta) % numMorserinoModes);
      // reset the encoder
      encoderDelta = 0;
      printTopMenu(morseState);
    }

  }
}

void printTopMenu(morserinoMode mode) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(F("Select Mode:"));
  lcd.setCursor(0, 1);
  lcd.print(mode + 1);
  lcd.print(" - ");
  switch (mode) {
    case morseKeyer:
      lcd.print(F("CW Keyer"));
      break;
    case morseTrainer:
      lcd.print(F("CW Trainer"));
      break;
    case morseDecoder:
      lcd.print(F("CW Decoder"));
      break;
    case morseCopyGame:
      lcd.print(F("Copy Game"));
      break;
    case morseQuickEcho:
      lcd.print(F("Quick Echo"));
      break;
#ifdef ENABLE_MEMO_KEYER
  case morseSetAutoKeyerText:
      lcd.print(F("Memorize"));
      break;
#endif
    case morseBacklight:
      lcd.print(F("Backlight "));
      lcd.print(CWsettings.backlightOn ? '*' : (char) 165);
      break;
  default:
    // this must never happen!
    lcd.print(F("Invalid mode"));
    break;
  }
}


// encoder subroutines

void isr ()  {
  static unsigned long encoderlastReadMicrosec;

  // Interrupt service routine is executed when a HIGH to LOW
  // transition is detected on CLK
#define ENCODER_DEBOUNCE_MICROSEC 11500
  if (micros() < encoderlastReadMicrosec + ENCODER_DEBOUNCE_MICROSEC)
    return;
  boolean up;

  if (digitalRead(PinCLK))
    up = digitalRead(PinDT);
  else
    up = !digitalRead(PinDT);
  encoderlastReadMicrosec = micros();
  encoderDelta = (up ? -1 : +1);
}

void saveConfig () {
  // Save configuration to EEPROM, only if value has changed, to
  // increase EEPROM life.
  EEPROM.updateByte(addressSignature, MorserSignature);
  EEPROM.updateBlock(addressCWsettings, CWsettings);
}

////////////////////////////
// Routines for morse decoder - to a certain degree based on code by
// Hjalmar Skovholm Hansen OZ1JHM - copyleft licence
///////////////////////////

void setupGoertzel () {
  // based on sampling_freq and target_freq, pre-compute some values that are
  // compute-intensive and won't change anyway
  //float bw = sampling_freq / nBuf;
  int k = (int) (0.5 + nBuf * target_freq / sampling_freq);
  float omega = 2.0 * PI * k / nBuf;
  //float sin_omega = sin(omega);
  coeff = 2.0 * cos(omega);
}


boolean checkTone() {
  // check if we have a tone signal at A6 with Gortzel's algorithm,
  // and apply some noise blanking as well.  the result will be in
  // globale variable filteredState.  we return true when we detected
  // a change in state, false otherwise!
  static boolean realstate = false;
  static boolean realstatebefore = false;
  static unsigned long stateLastChangedTime = 0;

  // the maximum of the magnitude seen
  static int magnitudeUpperLimit = 100 * 100;
  // FIXME this is an hardcoded constant number?! What does it mean
  static int magnitudeLowerLimit = 100 * 100;

  if (!digitalRead(straightPin) || checkPaddles()) {
    realstate = true;
    keyTx = true;
  }
  else {
    realstate = false;
    keyTx = false;
    // fill the buffer with samples
    for (int16_t i = 0; i < nBuf; i++)
      audioBuf[i] = analogRead(audioInPin);

    float Q0;
    // the value of Q0 at the last timestep
    float Q1 = 0;
    // the value of Q0 at the second to last timestep
    float Q2 = 0;
    for (int16_t i = 0; i < nBuf; i++) {
      // for every sample, compute...
      Q0 = coeff * Q1 - Q2 + (float) audioBuf[i];
      Q2 = Q1;
      Q1 = Q0;
    }
    // target_freq_coeff_real = (Q1 - Q2 * cos_omega)
    // target_freq_coeff_imag = (Q2 * sin_omega)
    float magnitudeSquared = Q1 * Q1 + Q2 * Q2 - Q1 * Q2 * coeff;

    // no need to take the sqrt, since the square serves just as well
    // as a measure of intensity.
    // magnitude = sqrt(magnitudeSquared);
    float magnitude = magnitudeSquared;

    // adjust the magnitude limit automatically
    if (magnitude > magnitudeLowerLimit) {
      // moving average filter
      magnitudeUpperLimit = magnitudeUpperLimit + (magnitude - magnitudeUpperLimit) / MAGNITUDE_AUTO_ADJUST_N;
    }

    if (magnitudeUpperLimit < magnitudeLowerLimit) {
      // FIXME Does this limit the sensitivity?
      magnitudeUpperLimit = magnitudeLowerLimit;
    }

    // check for state transitions in the magnitude
    realstate = magnitude > magnitudeUpperLimit * MAGNITUDE_SAFETY_FACTOR;
  }

  // clean up the state with a noise blanker (debouncing):
  // a state must be present for at least noiseBlankerTime, to be accepted.
  if (realstate != realstatebefore)
    stateLastChangedTime = millis();
  if (millis() - stateLastChangedTime > noiseBlankerTime) {
    filteredState = realstate;
  }
  realstatebefore = realstate;

  if (filteredState == filteredStateBefore)
    // no change detected in filteredState
    return false;
  else {
    filteredStateBefore = filteredState;
    // change detected in filteredState
    return true;
  }
}


void doDecode() {
  float lacktime;
  int wpm;

  switch(decoderState) {
  case INTERELEMENT_: if (checkTone()) {
      ON_();
      decoderState = HIGH_;
    } else {
      // we store the length of the pause
      lowDuration = millis() - startTimeLow;

      //  when high speeds we have to have a
      //  little more pause before new letter
      lacktime = 3.0;
      if (CWsettings.wpm > 35) lacktime = 3.2;
      else if (CWsettings.wpm > 30) lacktime = 3.4;
      if (lowDuration > (lacktime * ditAvg)) {
        // decode the morse character and display it
        displayMorse();
        // recalculate speed in wpm
        wpm = (CWsettings.wpm + (int) (7200 / (dahAvg + 3*ditAvg))) / 2;
        if (CWsettings.wpm != wpm) {
          CWsettings.wpm = wpm;
          speedChanged = true;
        }
        decoderState = INTERCHAR_;
      }
    }
    break;
  case INTERCHAR_:    if (checkTone()) {
      ON_();
      decoderState = HIGH_;
    } else {
      // we record the length of the pause
      lowDuration = millis() - startTimeLow;
      //  when high speeds we have to have a
      //  little more pause before new word
      lacktime = 7.0;
      if (CWsettings.wpm > 35) lacktime = 8.0;
      else if (CWsettings.wpm > 30) lacktime = 9.0;
      if (lowDuration > (lacktime * ditAvg)) {
        // since the blank is not in the linklist at the moment, one
        // cannot point the treeptr to it...
        treeptr = CWTREE_BLANK_SIG;
        displayMorse();
        decoderState = LOW_;
      }
    }
    break;
  case LOW_:
    if (checkTone()) {
      ON_();
      decoderState = HIGH_;
    }
    break;
  case HIGH_:
    if (checkTone()) {
      OFF_();
      decoderState = INTERELEMENT_;
    }
    break;
  }
}

void ON_() {
  // what we do when we just detected a rising flank, from low to high
  unsigned long timeNow = millis();
  lowDuration = timeNow - startTimeLow;
  // we record the length of the pause
  startTimeHigh = timeNow;
  // prime the timer for the high state
  vol.tone( sidetonePin, 744, realVolume[CWsettings.sidetoneVolume] );
  // start generating side tone
  if (keyTx)
    // turn the LED on, key transmitter, or whatever
    digitalWrite(keyerPin, HIGH);


  lcd.setCursor(LCD_COLUMNS-2,0);
  lcd.write(255);
  if (lowDuration < ditAvg * 2.4)
    // if we had an inter-element pause,
    // use it to adjust speed
    recalculateDit(lowDuration);
}

void OFF_() {
  /// what we do when we just detected a falling flank, from high to low
  unsigned long timeNow = millis();
  unsigned int threshold = (int) ( ditAvg * sqrt( dahAvg / ditAvg));

  highDuration = timeNow - startTimeHigh;
  startTimeLow = timeNow;

  if (highDuration > (ditAvg * 0.5) && highDuration < (dahAvg * 2.5)) {
    // filter out VERY short and VERY long highs
    if (highDuration < threshold) { /// we got a dit -
      treeptr = CWtree[treeptr].dit;
      recalculateDit(highDuration);
    }
    else  {        /// we got a dah
      treeptr = CWtree[treeptr].dah;
      recalculateDah(highDuration);
    }
  }
  vol.noTone();                     // stop side tone
  digitalWrite(keyerPin, LOW);      // stop keying Tx
  lcd.setCursor(LCD_COLUMNS-2, 0);
  lcd.write(' ');

}

void recalculateDit(unsigned long duration) {
  // recalculate the average dit length
  static int rot = 0;
  static unsigned long collector;

  if (rot<4) {
    collector += duration;
    ++rot;
  } else {
    // equivalen to divide by eight - 4 old avg values plus the new 4 samples
    ditAvg = (4* ditAvg + collector) >> 3;
    rot = 0;
    collector = 0;
  }
}

void recalculateDah(unsigned long duration) {
  // recalculate the average dah length
  static int rot = 0;
  static unsigned long collector;

  if (duration > 2* dahAvg)   {
    // very rapid decrease in speed!

    // we adjust faster, ditAvg as well!
    dahAvg = (dahAvg + 2* duration) / 3;

    ditAvg = ditAvg/2 + dahAvg/6;
  }
  else {
    if (rot<2) {
      collector += duration;
      ++rot;
    } else {
      // equivalen to divide by four - 1 old avg dah values, plus
      // one computed from avg dit, plus the new 2 samples
      dahAvg = (3* ditAvg + dahAvg + collector) >> 2;
      rot = 0;
      collector = 0;
    }
  }
}

byte i2cScan() {
  // run a scanner to find out I2C address of LCD display - no other
  // i2c devices should be active during scan!  we just check for 0x27
  // - if this is the wrong address, we assume it has to be 0x3f, as
  // these are the two addresses, depending on the chip being used
  // (either 8574T or 8574AT)

  for (uint8_t address = 20; address < 96; address++ ) {
    // The i2c_scanner uses the return value of
    // the Write.endTransmission to see if
    // a device did acknowledge to the address.
    Wire.beginTransmission(address);

    if (!Wire.endTransmission()) {
#ifdef DEBUG
      /* Serial.print("I2C Device found on address (decimal): "); */
      /* Serial.println(address); */
#endif
      return address;
    }

  }

  // this should never happen! this was already in the scanning range.
  return 39;
}

void melody(unsigned int note1, unsigned int note2, unsigned int note3) {
#define NOTE_DURATION 200
  vol.tone(sidetonePin, note1,
           realVolume[CWsettings.sidetoneVolume]);
  delay(NOTE_DURATION);
  vol.tone(sidetonePin, note2,
           realVolume[CWsettings.sidetoneVolume]);
  delay(NOTE_DURATION);
  vol.tone(sidetonePin, note3,
           realVolume[CWsettings.sidetoneVolume]);
  delay(NOTE_DURATION);
  vol.noTone();
}

void clearCounters(){
  // clear counters, to be able to count the input signs now
  signCounter = 0;
  signCounter_inclBlanks = 0;
  charCounter = 0;
}

void quickEchoGoAhead(boolean fetchNewWord){
  // the biggest possible unsigned long number - do not
  // output extra spaces!
  interWordTimer = ~0ul;

  secondaryMode = SEC_MODE_GENERATE;
  if(fetchNewWord)
    prepNewGeneratorRun(false);
  else {
    //repeat last word
    icurrent_sig = 0;
    clearCounters();
  }

  quickEcho_wordDone = false;
  delay(1000);
}

void accelerate(boolean up){
  if(!random(8)) {
    if(up) {
      // generate faster, by reducing pauses.
      CWsettings.farnsworthMode = max(1, CWsettings.farnsworthMode-1);
    } else
      CWsettings.farnsworthMode = min(9, CWsettings.farnsworthMode+1);
  }
}

byte getGroupOf5Length(){
  return morseState == morseQuickEcho ? 3 : 5;
}

int16_t textbufModulo(int16_t i){
  return (i + TEXT_BUFFER_LENGTH) % TEXT_BUFFER_LENGTH;
}

void fullScreenMsg(const __FlashStringHelper* msg){
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(msg);
  delay(1500);
  lcd.clear();
}

void clearAndPrintScreen(const __FlashStringHelper* topmsg,
                         const __FlashStringHelper* bottommsg){
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(topmsg);
  lcd.setCursor(0, 1);
  lcd.print(bottommsg);
}
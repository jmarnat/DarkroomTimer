#include <Arduino.h>
#include <TM1637Display.h>

#define PIN_BIG_RED_BUT 3

#define PIN_ENCODER_BUT 5
#define PIN_ENCODER_CLK 6
#define PIN_ENCODER_DAT 7

//#define PIN_DISPLAY_CLK 7
//#define PIN_DISPLAY_DIO 8

//#define SEG_DP 0b10000000

//const uint8_t SEG_DONE[] = {
//  SEG_B | SEG_C | SEG_D | SEG_E | SEG_G,           // d
//  SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F,   // O
//  SEG_C | SEG_E | SEG_G,                           // n
//  SEG_A | SEG_D | SEG_E | SEG_F | SEG_G            // E
//};
//
//
//
//TM1637Display display(PIN_DISPLAY_CLK, PIN_DISPLAY_DIO);
//
int redButLast, redButCurr;
int rotButLast, rotButCurr;

int rotClkCurr, rotDatCurr;
int rotClkLast, rotDatLast;
int rotPosition;
// int rotValCurr, rotValLast;

//
float varSelectedTime;
int varSelectedTimeDecimals;

void setup() {
  pinMode(PIN_ENCODER_BUT, INPUT);

  rotClkLast = 0;
  rotDatLast = 0;
  rotPosition = 0;

  redButLast = digitalRead(PIN_BIG_RED_BUT);
  rotButLast = digitalRead(PIN_ENCODER_BUT);

  rotClkLast = digitalRead(PIN_ENCODER_CLK);

  Serial.begin(9600);
  
  
}

void loop() {
  // reading big red push button
  redButCurr = digitalRead(PIN_BIG_RED_BUT);
  if (redButCurr != redButLast) {
    if (redButCurr == HIGH) {
      Serial.println("RED BUT ON");
    } else {
      Serial.println("RED BUT off");
    }
    redButLast = redButCurr;
  }
  
  // reading rotary encoder push button
  rotButCurr = digitalRead(PIN_ENCODER_BUT);
  if (rotButCurr != rotButLast) {
    if (rotButCurr == LOW) {
      Serial.println("ROT BUT ON");
    } else {
      Serial.println("ROT BUT off");
    }
    rotButLast = rotButCurr;
  }

  // reading rotary encoder
  rotClkCurr = digitalRead(PIN_ENCODER_CLK);
  rotDatCurr = digitalRead(PIN_ENCODER_DAT);
  if (rotClkCurr != rotClkLast) {
    if (rotClkCurr != rotDatCurr) {
      rotPosition ++;
    } else {
      rotPosition --;
    }

    
    if ((rotPosition % 2) == 0) {
      Serial.print("ROT POSITION: ");
      Serial.println(rotPosition/2);
    }
    

    rotClkLast = rotClkCurr;
  }

}

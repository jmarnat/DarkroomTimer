#include <Arduino.h>
#include <TM1637Display.h>

#define PIN_BIG_RED_BUT 2

#define PIN_ENCODER_BUT 4
#define PIN_ENCODER_CLK 5
#define PIN_ENCODER_DAT 6

#define PIN_DISPLAY_CLK 7
#define PIN_DISPLAY_DIO 8

#define SEG_DP 0b10000000

const uint8_t SEG_DONE[] = {
  SEG_B | SEG_C | SEG_D | SEG_E | SEG_G,           // d
  SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F,   // O
  SEG_C | SEG_E | SEG_G,                           // n
  SEG_A | SEG_D | SEG_E | SEG_F | SEG_G            // E
};



TM1637Display display(PIN_DISPLAY_CLK, PIN_DISPLAY_DIO);

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


  // display
  varSelectedTime = 2.5;
  varSelectedTimeDecimals = 1;
  
  display.setBrightness(0x0f);
  displayDigits(varSelectedTime, varSelectedTimeDecimals);
  
  
}

void displayDigits(float number, int decimals) {
  if (decimals == 0) {
    display.showNumberDec(varSelectedTime);
    return;
  }
  if (decimals == 1) {
    uint8_t data[] = { 0x00, 0x00, 0x00, 0x00};
    int nb_int = int(number * 10);
    int d0 = (nb_int % 10000) / 1000;
    int d1 = (nb_int % 1000) / 100;
    int d2 = (nb_int % 100) / 10;
    int d3 = (nb_int % 10);
    data[0] = display.encodeDigit(d0);
    data[1] = display.encodeDigit(d1);
    data[2] = display.encodeDigit(d2);
    data[3] = display.encodeDigit(d3);

//    Serial.print("Digits: ");
//    Serial.print(d0);
//    Serial.print(",");
//    Serial.print(d1);
//    Serial.print(",");
//    Serial.print(d2);
//    Serial.print(",");
//    Serial.print(d3);
//    Serial.println();
    
    // adding digital point
    data[2] = data[2] | SEG_DP;
    display.setSegments(data);
  }
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
      int trueValue = rotPosition/2;
      
      if ((trueValue > 0) and (trueValue < 9999)) {
        varSelectedTime = (float) trueValue / 10;
        // display.showNumberDec(varSelectedTime, varSelectedTimeDecimals);
        displayDigits(varSelectedTime, varSelectedTimeDecimals);
      }
      
      
    }
    

    rotClkLast = rotClkCurr;
  }


  /*
    int k;
    uint8_t data[] = { 0xff, 0xff, 0xff, 0xff };
    uint8_t blank[] = { 0x00, 0x00, 0x00, 0x00 };
    display.setBrightness(0x0f);

    // All segments on
    display.setSegments(data);
    delay(TEST_DELAY);

    // Selectively set different digits
    data[0] = display.encodeDigit(0);
    data[1] = display.encodeDigit(1);
    data[2] = display.encodeDigit(2);
    data[3] = display.encodeDigit(3);
    display.setSegments(data);
    delay(TEST_DELAY);

    display.clear();
    display.setSegments(data+2, 2, 2);
    delay(TEST_DELAY);

    display.clear();
    display.setSegments(data+2, 2, 1);
    delay(TEST_DELAY);

    display.clear();
    display.setSegments(data+1, 3, 1);
    delay(TEST_DELAY);


    // Show decimal numbers with/without leading zeros
    display.showNumberDec(0, false); // Expect: ___0
    delay(TEST_DELAY);
    display.showNumberDec(0, true);  // Expect: 0000
    delay(TEST_DELAY);
    display.showNumberDec(1, false); // Expect: ___1
    delay(TEST_DELAY);
    display.showNumberDec(1, true);  // Expect: 0001
    delay(TEST_DELAY);
    display.showNumberDec(301, false); // Expect: _301
    delay(TEST_DELAY);
    display.showNumberDec(301, true); // Expect: 0301
    delay(TEST_DELAY);
    display.clear();
    display.showNumberDec(14, false, 2, 1); // Expect: _14_
    delay(TEST_DELAY);
    display.clear();
    display.showNumberDec(4, true, 2, 2);  // Expect: 04__
    delay(TEST_DELAY);
    display.showNumberDec(-1, false);  // Expect: __-1
    delay(TEST_DELAY);
    display.showNumberDec(-12);        // Expect: _-12
    delay(TEST_DELAY);
    display.showNumberDec(-999);       // Expect: -999
    delay(TEST_DELAY);
    display.clear();
    display.showNumberDec(-5, false, 3, 0); // Expect: _-5_
    delay(TEST_DELAY);
    display.showNumberHexEx(0xf1af);        // Expect: f1Af
    delay(TEST_DELAY);
    display.showNumberHexEx(0x2c);          // Expect: __2C
    delay(TEST_DELAY);
    display.showNumberHexEx(0xd1, 0, true); // Expect: 00d1
    delay(TEST_DELAY);
    display.clear();
    display.showNumberHexEx(0xd1, 0, true, 2); // Expect: d1__
    delay(TEST_DELAY);

    // Run through all the dots
    for(k=0; k <= 4; k++) {
  	display.showNumberDecEx(0, (0x80 >> k), true);
  	delay(TEST_DELAY);
    }

    // Brightness Test
    for(k = 0; k < 4; k++)
    data[k] = 0xff;
    for(k = 0; k < 7; k++) {
    display.setBrightness(k);
    display.setSegments(data);
    delay(TEST_DELAY);
    }

    // On/Off test
    for(k = 0; k < 4; k++) {
    display.setBrightness(7, false);  // Turn off
    display.setSegments(data);
    delay(TEST_DELAY);
    display.setBrightness(7, true); // Turn on
    display.setSegments(data);
    delay(TEST_DELAY);
    }


    // Done!
    display.setSegments(SEG_DONE);
  */

//  displayDigits(0, 1);
//  delay(1000);
//  displayDigits(0.5, 1);
//  delay(1000);
//  displayDigits(2, 1);
//  delay(1000);
//  displayDigits(2.5, 1);
//  delay(1000);

//  displayDigits(0, 2);
//  delay(1000);
//  displayDigits(0.25, 2);
//  delay(1000);
//  displayDigits(2, 2);
//  delay(1000);
//  displayDigits(2.75, 2);
//  delay(1000);
}

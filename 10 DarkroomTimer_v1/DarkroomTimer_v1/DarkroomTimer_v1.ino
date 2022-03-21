#include <Arduino.h>
#include <TM1637Display.h>


#define PIN_BIG_RED_BUT 3

#define PIN_ENCODER_BUT 5
#define PIN_ENCODER_CLK 6
#define PIN_ENCODER_DAT 7

#define PIN_RELAY_RED  8

#define PIN_LEFT_DISPLAY_CLK  9
#define PIN_LEFT_DISPLAY_DIO 10

#define PIN_RIGHT_DISPLAY_CLK 11
#define PIN_RIGHT_DISPLAY_DIO 12



#define MODE_SETUP 0
#define MODE_TIME_RUN 1
#define MODE_TIME_PAUSE 2
#define MODE_MENU 3




#define SEG_DP 0b10000000

//#define LETTER_D
//#define LETTER_E 0b01111001
//#define LETTER_I 0b00000100
//#define LETTER_M 0b00110111
//#define LETTER_N 0b01010100
//#define LETTER_O 
//#define LETTER_T 0b01111000
//#define LETTER_U 0b00011100

const uint8_t SEG_DONE[] = {
  SEG_B | SEG_C | SEG_D | SEG_E | SEG_G,           // d
  SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F,   // O
  SEG_C | SEG_E | SEG_G,                           // n
  SEG_A | SEG_D | SEG_E | SEG_F | SEG_G            // E
};

const uint8_t SEG_TIME[] = {
  0b01111000,
  0b00000100,
  0b00110111,
  0b01111001
};

const uint8_t SEG_MENU[] = {
  0b00110111,
  0b01111001,
  0b01010100,
  0b00011100
};



TM1637Display display_left(PIN_LEFT_DISPLAY_CLK, PIN_LEFT_DISPLAY_DIO);
TM1637Display display_right(PIN_RIGHT_DISPLAY_CLK, PIN_RIGHT_DISPLAY_CLK);

int redButLast, redButCurr;
int rotButLast, rotButCurr;

int rotClkCurr, rotDatCurr;
int rotClkLast, rotDatLast;
int rotPosition;
// int rotValCurr, rotValLast;

int lightCurr, lightLast;


//
float varSelectedTime;
float timerTime;
int varSelectedTimeDecimals;


///
int mode = MODE_SETUP;
unsigned long currMillis, lastMillis;

void setup() {
  pinMode(PIN_ENCODER_BUT, INPUT);
  pinMode(PIN_BIG_RED_BUT, INPUT);
  pinMode(PIN_RELAY_RED, OUTPUT);

  rotClkLast = 0;
  rotDatLast = 0;
  rotPosition = 0;
//  rotPosLast = 0;

  redButLast = digitalRead(PIN_BIG_RED_BUT);
  rotButLast = digitalRead(PIN_ENCODER_BUT);

  rotClkLast = digitalRead(PIN_ENCODER_CLK);

  digitalWrite(PIN_RELAY_RED, HIGH);

//  lightLast = analogRead(PIN_POT_LIGHT) / 128;


  Serial.begin(9600);


  // display
  varSelectedTime = 2.5;
  varSelectedTimeDecimals = 1;

  display.setBrightness(0x0f);
  displayDigits(varSelectedTime, varSelectedTimeDecimals);


  // stopTimer();

}

void displayDigits(float number, int decimals) {
  if (decimals == 0) {
    display.showNumberDec(number);
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

void lightOn() {
  digitalWrite(PIN_RELAY_RED, LOW);
}

void lightOff() {
  digitalWrite(PIN_RELAY_RED, HIGH);
}

void startTimer(boolean restart) {
  mode = MODE_TIME_RUN;
  if (restart) {
    timerTime = varSelectedTime;
  }
  lastMillis = millis();
  lightOn();
}

void pauseTimer() {
  mode = MODE_TIME_PAUSE;
  // timerTime = varSelectedTime;
  // lastMillis = millis();
  lightOff();
}

void stopTimer() {
  mode = MODE_SETUP;
  lightOff();
  displayDigits(varSelectedTime, varSelectedTimeDecimals);
}


void setBrightness(byte val) {
  uint8_t data[] = { 0xff, 0xff, 0xff, 0xff };
  display.setBrightness(val);
  display.setSegments(data);
}

void loop() {
  // reading big red push button
  redButCurr = digitalRead(PIN_BIG_RED_BUT);
  if (redButCurr != redButLast) {
    if (redButCurr == HIGH) {
      Serial.println("RED BUT ON");

      if (mode == MODE_SETUP) {
        startTimer(true);
      } else if (mode == MODE_TIME_PAUSE) {
        startTimer(false);
      }

    } else {
      Serial.println("RED BUT off");
      if (mode == MODE_TIME_RUN) {
        pauseTimer();
      }
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
      if (rotPosition > 0) {
        varSelectedTime += 1;
        Serial.println(varSelectedTime);
      } else if (varSelectedTime > 1) {
        varSelectedTime -= 1;
        Serial.println(varSelectedTime);
      }
      displayDigits(varSelectedTime, varSelectedTimeDecimals);
      rotPosition = 0;
//      rotPosition
      
//      Serial.print("ROT POSITION: ");
//      Serial.println(rotPosition / 2);
//      int trueValue = rotPosition / 2;

//      if ((trueValue > 0) and (trueValue < 9999)) {
//        // varSelectedTime = (float) trueValue / 10;
//
//        if (varSelectedTime >= 10) {
//          varSelectedTimeDecimals = 0;
//        } else {
//          varSelectedTimeDecimals = 1;
//        }
//
//        // display.showNumberDec(varSelectedTime, varSelectedTimeDecimals);
//        displayDigits(varSelectedTime, varSelectedTimeDecimals);
//      }


    }


    rotClkLast = rotClkCurr;
    
  }


  // reading pot light
//  lightCurr = map(analogRead(PIN_POT_LIGHT),0,1023,0,7);
//  if (lightCurr != lightLast) {
//    Serial.print("New light: ");
//    Serial.println(lightCurr);
//    setBrightness(lightCurr);
//    lightLast = lightCurr;
//  }

  if (mode == MODE_TIME_RUN) {
    currMillis = millis();
    // only check every 0.1s
    if (currMillis > (lastMillis + 100)) {
      timerTime -= 0.1;
      lastMillis = currMillis;
      displayDigits(timerTime, varSelectedTimeDecimals);
    }

    if (timerTime <= 0) {
      stopTimer();
    }
  }
}

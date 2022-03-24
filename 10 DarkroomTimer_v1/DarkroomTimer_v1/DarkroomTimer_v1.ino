#include <Arduino.h>
#include <TM1637Display.h>


#define PIN_MIDDLE_BUT  2
#define PIN_BIG_RED_BUT 3

#define PIN_PIEZO 4

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
#define MODE_FOCUS 3

#define MODE_MENU            10 // initial menu

#define MODE_MENU_MODE       20 // mode time/f-stop
#define MODE_MENU_MODE_TIME  21 // .. time
#define MODE_MENU_MODE_FSTOP 22 // .. f-stop

#define MODE_MENU_FSTP       30 // f-stop
#define MODE_MENU_FLTR       40 // filter
#define MODE_MENU_DIST       50 // distance
#define MODE_MENU_STRP       60 // test strip generator

#define MODE_MENU_PRMS       70 // parameters
#define MODE_MENU_PRMS_BRGT  71 // parameters
#define MODE_MENU_PRMS_BIPA  72 // parameters
#define MODE_MENU_PRMS_BIPB  73 // parameters
#define MODE_MENU_PRMS_BIPC  74 // parameters
#define MODE_MENU_PRMS_BIPD  75 // parameters


//#define MODE_PIEZO_FREQ
//#define MODE_PIEZO_DURATION 6




#define SEG_DP 0b10000000

//#define LETTER_D


#define LETTER_A 0b01011111
#define LETTER_B 0b01111100
#define LETTER_C 0b00111001
#define LETTER_D 0b01011110
#define LETTER_E 0b01111001
#define LETTER_F 0b01110001
#define LETTER_G 0b00111101
#define LETTER_I 0b00000100
#define LETTER_K 0b01110101
#define LETTER_L 0b00111000
#define LETTER_M 0b00110111
#define LETTER_N 0b01010100
#define LETTER_O 0b00111111
#define LETTER_P 0b01110011
#define LETTER_R 0b01010000
#define LETTER_S 0b01101101
#define LETTER_T 0b01111000
#define LETTER_U 0b00011100

int current_fstop = 2; // CAUTION : THIS IS THE F-STOP INDEX, NOT THE TRUE VALUE (ie 2 = f/4)
const float F_STOPS[] = {
  2,
  2.8,
  4,
  5.6,
  8,
  11,
  16,
  22
};

const uint8_t SEG_DIGITS[] = {
  0b00111111, // 0
  0b00000110, // 1
  0b01011011, // 2
  0b01001111, // 3
  0b01100110, // 4
  0b01101101, // 5
  0b01111101, // 6
  0b00000111, // 7
  0b01111111, // 8
  0b01101111 // 9
};

const uint8_t SEG_DONE[] = {
  SEG_B | SEG_C | SEG_D | SEG_E | SEG_G,           // d
  SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F,   // O
  SEG_C | SEG_E | SEG_G,                           // n
  SEG_A | SEG_D | SEG_E | SEG_F | SEG_G            // E
};

const uint8_t SEG_TIME[] = {LETTER_T, LETTER_I, LETTER_M, LETTER_E};
const uint8_t SEG_MENU[] = {LETTER_M, LETTER_E, LETTER_N, LETTER_U};
const uint8_t SEG_MODE[] = {LETTER_M, LETTER_O, LETTER_D, LETTER_E};
const uint8_t SEG_RUN[]  = {LETTER_R, LETTER_U, LETTER_N,        0};
const uint8_t SEG_STOP[] = {LETTER_S, LETTER_T, LETTER_O, LETTER_P};
const uint8_t SEG_BRGT[] = {LETTER_B, LETTER_R, LETTER_G, LETTER_T};
const uint8_t SEG_BACK[] = {LETTER_B, LETTER_A, LETTER_C, LETTER_K};
const uint8_t SEG_FSTP[] = {LETTER_F, LETTER_S, LETTER_T, LETTER_P};
const uint8_t SEG_FLTR[] = {LETTER_F, LETTER_L, LETTER_T, LETTER_R};

const uint8_t SEG_DIST[] = {LETTER_D, LETTER_I, LETTER_S, LETTER_T};
const uint8_t SEG_STRP[] = {LETTER_S, LETTER_T, LETTER_R, LETTER_P};
const uint8_t SEG_PRMS[] = {LETTER_P, LETTER_R, LETTER_M, LETTER_S};

const uint8_t SEG_BIP1[] = {LETTER_B, LETTER_I, LETTER_P, SEG_DIGITS[1]};
const uint8_t SEG_BIP2[] = {LETTER_B, LETTER_I, LETTER_P, SEG_DIGITS[2]};
const uint8_t SEG_BIP3[] = {LETTER_B, LETTER_I, LETTER_P, SEG_DIGITS[3]};
const uint8_t SEG_BIP4[] = {LETTER_B, LETTER_I, LETTER_P, SEG_DIGITS[4]};

//SEG_FLTR

TM1637Display display_left(PIN_LEFT_DISPLAY_CLK, PIN_LEFT_DISPLAY_DIO);
TM1637Display display_right(PIN_RIGHT_DISPLAY_CLK, PIN_RIGHT_DISPLAY_DIO);


int midButLast, midButCurr;
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
//int currentMode =
int submode;
unsigned long currMillis, lastMillis;

byte settings_brightness = 1;
bool settings_beeps_startup = true;
bool settings_beep1 = true;
bool settings_beep2 = true;
bool settings_beep3 = true;
bool settings_beep4 = true;

void setup() {
  pinMode(PIN_ENCODER_BUT, INPUT);
  pinMode(PIN_MIDDLE_BUT, INPUT);
  pinMode(PIN_BIG_RED_BUT, INPUT);
  pinMode(PIN_RELAY_RED, OUTPUT);

  rotClkLast = 0;
  rotDatLast = 0;
  rotPosition = 0;
  //  rotPosLast = 0;

  midButLast = digitalRead(PIN_MIDDLE_BUT);
  redButLast = digitalRead(PIN_BIG_RED_BUT);
  rotButLast = digitalRead(PIN_ENCODER_BUT);

  rotClkLast = digitalRead(PIN_ENCODER_CLK);

  digitalWrite(PIN_RELAY_RED, HIGH);

  Serial.begin(9600);


  // display
  varSelectedTime = 2.5;
  varSelectedTimeDecimals = 1;

  display_left.setBrightness(settings_brightness);
  setMode(MODE_SETUP);

  display_right.setBrightness(settings_brightness);
  displaySelectedTime();

  if (settings_beeps_startup) {
    tone(PIN_PIEZO, 440, 100);
    delay(100);
    tone(PIN_PIEZO, 554.37, 100);
    delay(100);
    tone(PIN_PIEZO, 659.25, 100);
    delay(100);
    tone(PIN_PIEZO, 880, 400);
  }
}

void lightOn() {
  digitalWrite(PIN_RELAY_RED, LOW);
}

void lightOff() {
  digitalWrite(PIN_RELAY_RED, HIGH);
}

// rotary beep
void beep1() {
  if (settings_beep1) {
    tone(PIN_PIEZO, 440 * 3, 2);
  }
}

// tic tac beep
void beep2() {
  if (settings_beep2) {
    tone(PIN_PIEZO, 440, 20);
  }
}

// finish beep
void beep3() {
  if (settings_beep3) {
    tone(PIN_PIEZO, 440 * 2, 500);
  }
}

// error beep
void beep4() {
  if (settings_beep3) {
    tone(PIN_PIEZO, 314, 200);
  }
}

void startTimer(boolean restart) {
  //  mode = MODE_TIME_RUN;
  if (restart) {
    timerTime = varSelectedTime;
  }
  lastMillis = millis();
  lightOn();
}

void pauseTimer() {
  //  mode = MODE_TIME_PAUSE;
  // timerTime = varSelectedTime;
  // lastMillis = millis();
  lightOff();
}

void stopTimer() {
  //  mode = MODE_SETUP;
  lightOff();
  displaySelectedTime();
}




//void getSubModes(int mode) {
//
//}

void setMode(int newMode) {
  mode = newMode;
  if (mode == MODE_SETUP) {
    display_left.setSegments(SEG_TIME);
    displaySelectedTime();

  } else if (mode == MODE_TIME_RUN) {
    display_left.setSegments(SEG_RUN);

  } else if (mode == MODE_TIME_PAUSE) {
    display_left.setSegments(SEG_STOP);

  } else if (mode == MODE_MENU) {
    submode = MODE_MENU_MODE;
    display_left.setSegments(SEG_MENU);
    display_right.setSegments(SEG_MODE);

  } else if (mode == MODE_MENU_MODE) {
    submode = MODE_MENU_MODE_TIME;
    display_left.setSegments(SEG_MENU);
    display_right.setSegments(SEG_TIME);

  } else if (mode == MODE_MENU_FSTP) {
    display_left.setSegments(SEG_FSTP);
    // display_left.setSegments(SEG_MENU);
    displayFstop(current_fstop);

  } else if (mode == MODE_MENU_PRMS) {
    submode = MODE_MENU_PRMS_BRGT;
    display_left.setSegments(SEG_PRMS);
    display_right.setSegments(SEG_BRGT);

  } else if (mode == MODE_MENU_PRMS_BRGT) {
    display_left.setSegments(SEG_BRGT);
    displayBrightness();
    /// display_right.setSegments(displayBrightness);

  } else if (mode == MODE_FOCUS) {
    // display_left.setSegments(SEG_MENU);
  }
}


void displayRemainingTime() {
  displayDigits(1, timerTime, varSelectedTimeDecimals);
}

void displaySelectedTime() {
  displayDigits(1, varSelectedTime, varSelectedTimeDecimals);
}

void displayDigits(int side, float number, int decimals) {
  if (decimals == 0) {
    if (side == 0)
      display_left.showNumberDec(number);
    else
      display_right.showNumberDec(number);
    return;
  }
  if (decimals == 1) {
    uint8_t data[] = { 0x00, 0x00, 0x00, 0x00};
    int nb_int = int(number * 10);
    int digits[] = {0, 0, 0, 0};

    digits[0] = (nb_int % 10000) / 1000;
    digits[1] = (nb_int % 1000) / 100;
    digits[2] = (nb_int % 100) / 10;
    digits[3] = (nb_int % 10);

    bool pad = true;
    for (int i = 0; i < 4; i++) {
      if ((digits[i] == 0) & pad) {
        data[i] = 0b00000000;
      } else {
        data[i] = display_right.encodeDigit(digits[i]);
        pad = false;
      }
    }

    // adding digital point
    data[2] = data[2] | SEG_DP;
    if (side == 0) display_left.setSegments(data);
    else if (side == 1) display_right.setSegments(data);
  }
}

void displayFstop(int fstop) {
  uint8_t data[] = {LETTER_F, 0x00, 0x00, 0x00};
  int smax = sizeof(F_STOPS) / sizeof(F_STOPS[0]);
  if (fstop < smax) {
    int fval = F_STOPS[fstop] * 10;
    int d0 = (fval / 100) % 10;
    int d1 = (fval / 10) % 10;
    int d2 = fval % 10;
    if (d0 > 0) data[1] = display_right.encodeDigit(d0);
    data[2] = display_right.encodeDigit(d1) | 0b10000000;
    data[3] = display_right.encodeDigit(d2);
    display_right.setSegments(data);
  }
}

void displayBrightness() {
  // settings_brightness
  display_left.setBrightness(settings_brightness);
  display_right.setBrightness(settings_brightness);
  display_left.setSegments(SEG_BRGT);
  displayDigits(1, settings_brightness + 1, 0);
}

//void setBrightness(byte val) {
//  uint8_t data[] = { 0xff, 0xff, 0xff, 0xff };
//  display.setBrightness(val);
//  display.setSegments(data);
//}


int selected_menu = 0;
void actionRotary(int dir) {
  // dir = +1 (clockwise) / -1 (trigo)

  if (mode == MODE_SETUP) {
    float varSelectedTimeNew = varSelectedTime + 1 * dir * 0.5;
    if (varSelectedTimeNew > 0) {
      varSelectedTime = varSelectedTimeNew;
      beep1();
    } else {
      // beep4();
    }

    displaySelectedTime();
    Serial.println(varSelectedTime);


  } else if (mode == MODE_MENU) {
    // to MODE_MENU_MODE
    if ((dir < 0) & (submode == MODE_MENU_FSTP)) {
      submode = MODE_MENU_MODE;
      display_right.setSegments(SEG_MODE);
      beep1();
    }

    // to MODE_MENU_FSTP
    else if (((dir > 0) and (submode == MODE_MENU_MODE)) or ((dir < 0) and (submode == MODE_MENU_FLTR))) {
      submode = MODE_MENU_FSTP;
      display_right.setSegments(SEG_FSTP);
      beep1();
    }

    // to MODE_MENU_FLTR
    else if (((dir > 0) and (submode == MODE_MENU_FSTP)) or ((dir < 0) and (submode == MODE_MENU_DIST))) {
      submode = MODE_MENU_FLTR;
      display_right.setSegments(SEG_FLTR);
      beep1();
    }

    // to MODE_MENU_DIST
    else if (((dir > 0) and (submode == MODE_MENU_FLTR)) or ((dir < 0) and (submode == MODE_MENU_STRP))) {
      submode = MODE_MENU_DIST;
      display_right.setSegments(SEG_DIST);
      beep1();
    }

    // to MODE_MENU_STRP
    else if (((dir > 0) and (submode == MODE_MENU_DIST)) or ((dir < 0) and (submode == MODE_MENU_PRMS))) {
      submode = MODE_MENU_STRP;
      display_right.setSegments(SEG_STRP);
      beep1();
    }

    // to MODE_MENU_PRMS
    else if ((dir > 0) and (submode == MODE_MENU_STRP)) {
      submode = MODE_MENU_PRMS;
      display_right.setSegments(SEG_PRMS);
      beep1();
    }

    else {
      // beep4();
    }


  } else if (mode == MODE_MENU_FSTP) {
    int smax = sizeof(F_STOPS) / sizeof(F_STOPS[0]);
    if (dir > 0) {
      if ((current_fstop + 1) < smax) {
        current_fstop++;
        displayFstop(current_fstop);
        beep1();
      } else {
        // beep4();
      }
    } else if (dir < 0) {
      if ((current_fstop - 1) >= 0) {
        current_fstop--;
        displayFstop(current_fstop);
        beep1();
      } else {
        // beep4();
      }
    }

  } else if (mode == MODE_MENU_PRMS) {
    //#define MODE_MENU_PRMS_BRGT  71 // parameters
    //#define MODE_MENU_PRMS_BIPA  72 // parameters
    //#define MODE_MENU_PRMS_BIPB  73 // parameters
    //#define MODE_MENU_PRMS_BIPC  74 // parameters
    //#define MODE_MENU_PRMS_BIPD  75 // parameters

    // -> MODE_MENU_PRMS_BRGT
    if ((dir < 0) and (submode == MODE_MENU_PRMS_BIPA)) {
      submode = MODE_MENU_PRMS_BRGT;
      display_right.setSegments(SEG_BRGT);
      beep1();
    }

    // to MODE_MENU_PRMS_BIPA
    else if (((dir > 0) and (submode == MODE_MENU_PRMS_BRGT)) or ((dir < 0) and (submode == MODE_MENU_PRMS_BIPB))) {
      submode = MODE_MENU_PRMS_BIPA;
      display_right.setSegments(SEG_BIP1);
      beep1();
    }

    // to MODE_MENU_PRMS_BIPB
    else if (((dir > 0) and (submode == MODE_MENU_PRMS_BIPA)) or ((dir < 0) and (submode == MODE_MENU_PRMS_BIPC))) {
      submode = MODE_MENU_PRMS_BIPB;
      display_right.setSegments(SEG_BIP2);
      beep1();
    }

    // to MODE_MENU_PRMS_BIPC
    else if (((dir > 0) and (submode == MODE_MENU_PRMS_BIPB)) or ((dir < 0) and (submode == MODE_MENU_PRMS_BIPD))) {
      submode = MODE_MENU_PRMS_BIPC;
      display_right.setSegments(SEG_BIP3);
      beep1();
    }

    // to MODE_MENU_PRMS_BIPD
    else if ((dir > 0) and (submode == MODE_MENU_PRMS_BIPC)) {
      submode = MODE_MENU_PRMS_BIPD;
      display_right.setSegments(SEG_BIP4);
      beep1();
    }

    else {
      // beep4();
    }

  } else if (mode == MODE_MENU_PRMS_BRGT) {
    if ((dir > 0) and (settings_brightness < 7)) {
      settings_brightness ++;
      displayBrightness();
      beep1();
    } else if ((dir < 0) and (settings_brightness > 0)) {
      settings_brightness--;
      displayBrightness();
      beep1();
    } else {
      // beep4();
    }


  } else {
    // beep4();
  }

}

void actionRotaryButton(boolean pushed) {
  if (pushed) {
    if (mode == MODE_SETUP) {
      setMode(MODE_MENU);

    }

    else if (mode == MODE_MENU) {
      setMode(submode);
    }

    else if (mode == MODE_MENU_PRMS) {
      setMode(submode);
    }



    else if (mode == MODE_TIME_PAUSE) {
      setMode(MODE_SETUP);
      stopTimer();
    }
  }
}

void actionBigRedButton(boolean pushed) {
  // pushed = 1, released = 0
  if (pushed) {
    // Serial.println("RED BUT ON");

    if (mode == MODE_SETUP) {
      setMode(MODE_TIME_RUN);
      startTimer(true);
    } else if (mode == MODE_TIME_RUN) {
      setMode(MODE_TIME_PAUSE);
      pauseTimer();
    } else if (mode == MODE_TIME_PAUSE) {
      setMode(MODE_TIME_RUN);
      startTimer(false);
    } else if (mode == MODE_MENU) {
      setMode(MODE_SETUP);
    }

    else if (
      (mode == MODE_MENU_FSTP)
      | (mode == MODE_MENU_FLTR)
      | (mode == MODE_MENU_DIST)
      | (mode == MODE_MENU_STRP)
      | (mode == MODE_MENU_PRMS)) {
      setMode(MODE_MENU);

    }

    else if ((mode == MODE_MENU_PRMS_BRGT)
             | (mode == MODE_MENU_PRMS_BIPA)
             | (mode == MODE_MENU_PRMS_BIPB)
             | (mode == MODE_MENU_PRMS_BIPC)
             | (mode == MODE_MENU_PRMS_BIPD)) {
      setMode(MODE_MENU_PRMS);
    }

    else {
      beep4();
    }
  } else {
    // Serial.println("RED BUT off");
  }
}



void loop() {
  /******************* READING INPUTS *******************/
  // reading middle switch
  midButCurr = digitalRead(PIN_MIDDLE_BUT);
  if (midButCurr != midButLast) {
    if (midButCurr == HIGH) {
      if (mode == MODE_SETUP) {
        Serial.println("MIDDLE SWITCH ON");
        setMode(MODE_FOCUS);
        lightOn();
      }
    } else {
      if (mode == MODE_FOCUS) {
        Serial.println("MIDDLE SWITCH OFF");
        setMode(MODE_SETUP);
        lightOff();
      }
    }
    midButLast = midButCurr;
  }

  // reading big red push button
  redButCurr = digitalRead(PIN_BIG_RED_BUT);
  if (redButCurr != redButLast) {
    actionBigRedButton(redButCurr == HIGH);
    redButLast = redButCurr;
  }

  // reading rotary encoder push button
  rotButCurr = digitalRead(PIN_ENCODER_BUT);
  if (rotButCurr != rotButLast) {
    actionRotaryButton(rotButCurr == LOW);
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
    Serial.print("rotPosition: ");
    Serial.println(rotPosition);

    if (rotPosition >= 2) {
      rotPosition -= 2;
      actionRotary(1);
    } else if (rotPosition <= -2) {
      rotPosition += 2;
      actionRotary(-1);
    }
    rotClkLast = rotClkCurr;

  }

  /************* NOW THE MAIN THREADED-LIKE LOOP *************/

  if (mode == MODE_TIME_RUN) {
    currMillis = millis();
    // only check every 0.1s
    if (currMillis > (lastMillis + 100)) {
      if ((((int) (timerTime * 10) % 10) == 0) & (timerTime > 0)) {
        beep2();
      }
      timerTime -= 0.1;
      lastMillis = currMillis;
      displayRemainingTime();
    }

    if (timerTime <= 0) {
      beep3();
      setMode(MODE_SETUP);
      stopTimer();
    }
  }
}

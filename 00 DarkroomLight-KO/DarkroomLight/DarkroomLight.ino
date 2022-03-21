#define ROTARY_CLK 4 // D2 - CLK pin of Rotary Encoder
#define ROTARY_DAT 5 // D1 - DT pin of Rotary Encoder

#define PIN_PWM_RED 14 // D5

#define MODE_STDBY 0
#define MODE_RED   1
#define MODE_WHT   2

int currentMode = MODE_RED;
int redIntensity = 32;
int whtIntensity = 32;

void adjustModeValue(int dir) {
  if (currentMode = MODE_RED) {
    if ((dir > 0) && (redIntensity < 1023)) {
      redIntensity *= 2;  
    } else if ((dir < 0) && (redIntensity > 2)) {
      redIntensity /= 2   ;
    }
    Serial.print("Red: ");
    Serial.println(redIntensity);

    analogWrite(PIN_PWM_RED, redIntensity);
  }
}

int prevRotClk = 0;

int getRotary() {
  int clk = digitalRead(ROTARY_CLK);
  int dat = digitalRead(ROTARY_DAT);
  int dir = 0;

  if (clk != prevRotClk) {
    if (clk != dat) dir = +1;
    else dir = -1;
    prevRotClk = clk;
  }
  return dir;
}

int getPush() {
  
}

void setup() {
  pinMode(ROTARY_CLK, INPUT);
  pinMode(ROTARY_DAT, INPUT);
  Serial.begin(9600);
}
void loop() {
  int dir = getRotary();
  if (dir != 0) {
    adjustModeValue(dir);
  }
}

#define PIN_BUT   2
#define PIN_PIEZO 4

#define PIN_POT_TONE     A0
#define PIN_POT_DURATION A1


int toneVal, durationVal;
int butVal, butLast;

void setup() {
  // put your setup code here, to run once:
  pinMode(PIN_BUT, INPUT);
  butLast = digitalRead(PIN_BUT);
  Serial.begin(9600);
}



void loop() {
  // put your main code here, to run repeatedly:
  // tone(PIN_PIEZO, 80, 500);
  // delay(2000);
  // noTone();
  butVal = digitalRead(PIN_BUT);
  if (butVal != butLast) {
    if (butVal == HIGH) {
      toneVal = map(analogRead(PIN_POT_TONE), 0, 1023, 80, 8000);
      durationVal = map(analogRead(PIN_POT_DURATION), 0, 1023, 10, 1000);
      Serial.print(toneVal);
      Serial.print("\t");
      Serial.println(durationVal);
      tone(PIN_PIEZO, toneVal, durationVal);
    }

    butLast = butVal;
  }

  
  
  
  
  
}

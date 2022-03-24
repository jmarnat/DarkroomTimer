
#define PIN_SWITCH 2

int lastVal, currVal;
void setup() {
  // put your setup code here, to run once:
  pinMode(PIN_SWITCH, INPUT);

  lastVal = digitalRead(PIN_SWITCH);
  Serial.begin(9600);
}



void loop() {
  currVal = digitalRead(PIN_SWITCH);
//  Serial.println(currVal);
  if (currVal != lastVal) {
  // put your main code here, to run repeatedly:
    Serial.println(currVal);
    lastVal = currVal;
  }
//  delay(200);
}

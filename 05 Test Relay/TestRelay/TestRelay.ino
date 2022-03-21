#define PIN_BIG_RED_BUT 2
#define PIN_RELAY_RED  10

int redLast = LOW;

void setup() {
  // put your setup code here, to run once:
  pinMode(PIN_BIG_RED_BUT, INPUT);
  pinMode(PIN_RELAY_RED, OUTPUT);
  digitalWrite(PIN_RELAY_RED, HIGH);
}


void loop() {
  // put your main code here, to run repeatedly:
  int r = digitalRead(PIN_BIG_RED_BUT);
  if (r != redLast) {
    if (r == HIGH) {
      digitalWrite(PIN_RELAY_RED, LOW);
    } else {
      digitalWrite(PIN_RELAY_RED, HIGH);
    }
    redLast = r;
  }
}

int buttonState = 0;
int LIGHTSENSOR = 0;
int PIRSENSORVAL = 0;
int RELAYOUTPUTVAL = 0;
int LIGHTSENSORVAL = 0;

void setup()
{
  pinMode(A0, INPUT);
  pinMode(2, INPUT);
  pinMode(8, INPUT);
  pinMode(8, OUTPUT);
  Serial.begin(9600);
}

void loop()
{
  LIGHTSENSORVAL = analogRead(A0);
  PIRSENSORVAL = digitalRead(2);
  RELAYOUTPUTVAL = digitalRead(8);
  if (LIGHTSENSORVAL < 600) {
    if (PIRSENSORVAL == HIGH) {
      digitalWrite(8, HIGH);
      delay(5000); // Wait for 5000 millisecond(s)
    } else {
      digitalWrite(8, LOW);
      delay(1000); // Wait for 1000 millisecond(s)
    }
  } else {
    digitalWrite(8, LOW);
    Serial.println(LIGHTSENSORVAL);
    delay(300); // Wait for 300 millisecond(s)
  }
}

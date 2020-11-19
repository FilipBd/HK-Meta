const int temppin = 0; // tempsensor A0
int16_t tempval = 0;

void setup(void) {
  Serial.begin(115200);
}

void loop(void) {
  tempval = analogRead(temppin);
  tempval = map(tempval, 687.3, 2839.70, -70, 500);
  Serial.println(tempval);
  delay(500);
}
const int temppin = 0; // tempsensor
int16_t tempval = 0;

void setup(void) {
  // put your setup code here, to run once:
  Serial.begin(115200);
}

void loop(void) {
  // put your main code here, to run repeatedly:
  tempval = analogRead(temppin);
  tempval = map(tempval, 687.3, 2839.70, -70, 500);
  Serial.println(tempval);
  delay(500);
}


void setup() {
  pinMode(2, INPUT);
  pinMode(3, INPUT);
  Serial.begin(115200);
}

// the loop() methor runs over and over again,
// as long as the board has power

void loop() {
  Serial.print(digitalRead(3)|(digitalRead(2)<<1));
  delay(10);
}


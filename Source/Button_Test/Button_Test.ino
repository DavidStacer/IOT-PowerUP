

#define BUTTON_PIN   17


void setup() {
  Serial.begin(115200);
  pinMode(BUTTON_PIN, INPUT_PULLUP);

}

void loop() {
  double begin, end;
  int count = 0;
  begin = millis();
  // Get current button state.

  while (1) {
    bool newState = digitalRead(BUTTON_PIN);
    if (!newState) {
      //Serial.println("switch pressed");
      count++;
    }
    if (millis() % 1000 == 0) {
      Serial.print(count); Serial.print("  ");
      Serial.println(millis());
      count = 0;
      delay(1);
    }
  }

}

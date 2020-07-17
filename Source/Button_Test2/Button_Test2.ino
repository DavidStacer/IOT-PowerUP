

#define BUTTON_PIN   17


void setup() {
  Serial.begin(115200);
  pinMode(BUTTON_PIN, INPUT_PULLUP);

}

void loop() {
  unsigned long begin, end;
  bool oldState = false ;
  int on_count = 0;
  int off_count = 0;
  begin = millis();
  // Get current button state.

  while (1) {
    bool newState = digitalRead(BUTTON_PIN);
    if ( newState != oldState) {
      if (!newState)
        on_count++;
      else
        off_count++;

      oldState  = newState;
    }
    if (!newState) {
      //Serial.println("switch pressed");
      on_count++;
    }
    if (millis() % 1000 == 0) {
      Serial.print("ON: ");Serial.print(on_count); Serial.print(" OFF:");Serial.print(off_count);Serial.print("  ");
      Serial.println(millis());
      on_count = 0;
      off_count = 0;
      delay(1);
    }
  }

}

// Include the Bounce2 library found here :
// https://github.com/thomasfredericks/Bounce2
#include <Bounce2.h>

#define BUTTON_PIN   17

// Instantiate a Bounce object
Bounce debouncer = Bounce(); 

void setup() {
  Serial.begin(115200);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  // After setting up the button, setup the Bounce instance :
  debouncer.attach(BUTTON_PIN);
  debouncer.interval(5); // interval in ms

}

void loop() {
  unsigned long begin, end;
  bool oldState = false ;
  int on_count = 0;
  int off_count = 0;
  begin = millis();
  
  while (1) {
    debouncer.update();
   
    if ( debouncer.rose()) 
        on_count++;

    if ( debouncer.fell())
        off_count++;


    if (millis() % 1000 == 0) {
      Serial.print("ON: ");Serial.print(on_count); Serial.print(" OFF:");Serial.print(off_count);Serial.print("  ");
      Serial.println(millis());
      on_count = 0;
      off_count = 0;
      delay(1);
    }
  }

}
